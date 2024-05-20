/**
 * @file    Emulator.cpp
 * @author  William Weston
 * @brief   Hack Emulator GUI Application
 * @version 0.1
 * @date 2024-03-15
 * 
 * @copyright Copyright (c) 2024
 * 
 * Note:  ImGui is NOT exception safe.
 *        To overcome this limitation, ImGuiSugar is used to provide an RAII wrapper around ImGui
 * 
 */
#include "Emulator.h"
#include "GUI_Frame.h"
#include "SDL_InitError.h"
#include "Utilities.h"

#include <Hack/Computer.h>
#include <Hack/Utilities/exceptions.hpp>
#include <Hack/Utilities/utilities.hpp>
#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include <ImGuiSugar/imgui_sugar.hpp>

#include <imgui.h>
#include <imgui_stdlib.h>              
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <SDL.h>

#include <charconv>        // from_chars
#include <cstdint> 
#include <exception>       // exception 
#include <expected>
#include <fstream> 
#include <iostream>
#include <iterator>
#include <stdexcept>       // out_of_range
#include <string>
#include <string_view>
#include <utility>         // move
#include <vector>

namespace 
{
   auto file_error_popup( std::string_view error_msg  )     -> bool;
   auto parse_error_popup( std::string_view error_msg )     -> bool;
   auto unsupported_filetype_popup( std::string_view path ) -> bool;
   auto memory_access_error_popup( std::string_view msg )   -> bool;
}

// -------------------------------------------- API -----------------------------------------------

Hack::Emulator::Emulator( std::string_view title, int x_pos, int y_pos, int width, int height, bool fullscreen )
   :  core_( title, x_pos, y_pos, width, height, fullscreen ),
      screen_texture_( computer_.screen_cbegin(), computer_.screen_cend(), core_.window(), core_.renderer() ),
      window_{ core_.window() },
      renderer_{ core_.renderer() }
{
   
   running_ = true;     // if initialization succeeds, running_ = true
}


auto
Hack::Emulator::run() -> void
{
   while ( is_running() )
   {
      try
      {
         auto const frame = GUI_Frame( core_.renderer() );
         handle_events();
         update();
         render();
      }
      catch( Hack::Utils::parse_error const& error )
      {
         std::cerr << "Error Parsing: " << current_file_ << '\n';
         std::cerr << error.what()  << '\n';
         std::cerr << error.where() << '\n';

         auto error_msg = "File:     " + current_file_ +  "\n" +
                          "Line no:  " + std::to_string( error.data().line_no ) + "\n" +
                          "Text:     " + error.data().text;

         user_error_ = UserError{ std::move( error_msg ), Error_t::PARSE_ERROR, true };
      }
      catch( Hack::EMULATOR::Utils::file_error const& error )
      {
         std::cerr << error.what()  << '\n';
         std::cerr << error.where() << '\n';
         
         user_error_ = UserError{ error.data().filename, Error_t::FILE_ERROR, true };
      }
      catch( Hack::EMULATOR::Utils::unsupported_filetype_error const& error )
      {
         std::cerr << error.what()  << '\n';
         std::cerr << error.where() << '\n';
         
         user_error_ = UserError{ std::move( current_file_ ), Error_t::UNSUPPORTED_FILETYPE, true };
      }
      catch( std::out_of_range const& error )
      {
         std::cerr << error.what() << '\n';

         auto error_msg = std::string();
   
         if ( computer_.pc() >= Computer::ROM_SIZE )
         {
            error_msg = "Illegal Memory Access to ROM at address: " + std::to_string( computer_.pc() ) + "\n" 
                      + "Stopping Hack Program";
         }
         else if( computer_.A_Register() >= Computer::RAM_SIZE )
         {
            error_msg = "Illegal Memory Access to RAM at address: " + std::to_string( computer_.A_Register() ) + "\n"
                      + "Stopping Hack Program";
         }
         else
         {
            error_msg = "Out of bounds";
         }

         user_error_ = UserError{ std::move( error_msg ), Error_t::MEMORY_ACCESS_ERROR, true };

         play_ = false;
         step_ = false;
         computer_.reset();
      }
      catch( std::exception const& error )
      {
         std::cerr << error.what() << '\n';
      }
   }
}


// ------------------------------------- Implementation -------------------------------------------

auto
Hack::Emulator::handle_events() -> void
{
   SDL_Event event;
   while ( SDL_PollEvent( &event ) )
   {
      ImGui_ImplSDL2_ProcessEvent( &event );

      if ( event.type == SDL_QUIT )
      {
         running_ = false;
      }

      if ( event.type            == SDL_WINDOWEVENT               && 
           event.window.event    == SDL_WINDOWEVENT_CLOSE         && 
           event.window.windowID == SDL_GetWindowID( window_ ) )
      { 
         running_ = false;
      }

      // Keyboard Events
      if ( event.type == SDL_KEYDOWN )
      {
         keyboard_handler_.onKeyDown(); 
      }

      if ( event.type == SDL_KEYUP )
      {
         keyboard_handler_.onKeyUp();
      }
   }

   handle_keyboard_events();
}


auto
Hack::Emulator::update() -> void
{   
   update_Hack_Computer();
   update_GUI_interface();

   screen_texture_.update();
}


auto 
Hack::Emulator::render() const -> void
{
}


auto 
Hack::Emulator::handle_keyboard_events() -> void
{
   // Hack has the behaviour that the most recently pressed key or key combination is put into the keyboard memory
   static constexpr auto null_key = SDL_SCANCODE_MENU;   // menu is unused, use as null key
   
   struct Key_Press
   {
      SDL_Scancode key   = null_key;
      bool         shift = false;
   };

   static Key_Press previous;
   
   // See Hack Character Set for keyboard mapping 

   // if the previous key has changed
   if ( keyboard_handler_.isKeyUp( previous.key ) || previous.shift != keyboard_handler_.isShiftDown() )
   {
      previous             = Key_Press{};
      computer_.keyboard() = 0;
   }


   if ( previous.key != SDL_SCANCODE_SPACE && keyboard_handler_.isKeyDown( SDL_SCANCODE_SPACE ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_SPACE, false };
      computer_.keyboard() = 32;                   // space
   }

   else if ( ( previous.key != SDL_SCANCODE_1 || previous.shift != false ) && 
               keyboard_handler_.isKeyDown( SDL_SCANCODE_1 ) && keyboard_handler_.isShiftUp() )
   {
      previous             = Key_Press{ SDL_SCANCODE_1, false };
      computer_.keyboard() = 49;                                  // 1
   }

   else if ( ( previous.key != SDL_SCANCODE_1 || previous.shift != true ) &&
               keyboard_handler_.isKeyDown( SDL_SCANCODE_1 ) && keyboard_handler_.isShiftDown() )
   {
      previous             = Key_Press{ SDL_SCANCODE_1, true };
      computer_.keyboard() = 33;                                  // !
   }

   else if ( ( previous.key != SDL_SCANCODE_2 || previous.shift != false ) && 
               keyboard_handler_.isKeyDown( SDL_SCANCODE_2 ) && keyboard_handler_.isShiftUp() )
   {
      previous             = Key_Press{ SDL_SCANCODE_2, false };
      computer_.keyboard() = 50;                                  // 2
   }

   else if ( ( previous.key != SDL_SCANCODE_2 || previous.shift != true ) &&
               keyboard_handler_.isKeyDown( SDL_SCANCODE_2 ) && keyboard_handler_.isShiftDown() )
   {
      previous             = Key_Press{ SDL_SCANCODE_2, true };
      computer_.keyboard() = 64;                                  // @
   }

   else if ( ( previous.key != SDL_SCANCODE_3 || previous.shift != false ) && 
               keyboard_handler_.isKeyDown( SDL_SCANCODE_3 ) && keyboard_handler_.isShiftUp() )
   {
      previous             = Key_Press{ SDL_SCANCODE_3, false };
      computer_.keyboard() = 51;                                  // 3
   }

   else if ( ( previous.key != SDL_SCANCODE_3 || previous.shift != true ) &&
               keyboard_handler_.isKeyDown( SDL_SCANCODE_3 ) && keyboard_handler_.isShiftDown() )
   {
      previous             = Key_Press{ SDL_SCANCODE_3, true };
      computer_.keyboard() = 35;                                  // #
   }

   else if ( ( previous.key != SDL_SCANCODE_4 || previous.shift != false ) && 
               keyboard_handler_.isKeyDown( SDL_SCANCODE_4 ) && keyboard_handler_.isShiftUp() )
   {
      previous             = Key_Press{ SDL_SCANCODE_4, false };
      computer_.keyboard() = 52;                                  // 4
   }

   else if ( ( previous.key != SDL_SCANCODE_4 || previous.shift != true ) &&
               keyboard_handler_.isKeyDown( SDL_SCANCODE_4 ) && keyboard_handler_.isShiftDown() )
   {
      previous             = Key_Press{ SDL_SCANCODE_4, true };
      computer_.keyboard() = 36;                                  // $
   }

   else if ( ( previous.key != SDL_SCANCODE_5 || previous.shift != false ) && 
               keyboard_handler_.isKeyDown( SDL_SCANCODE_5 ) && keyboard_handler_.isShiftUp() )
   {
      previous             = Key_Press{ SDL_SCANCODE_5, false };
      computer_.keyboard() = 53;                                  // 5
   }

   else if ( ( previous.key != SDL_SCANCODE_5 || previous.shift != true ) &&
               keyboard_handler_.isKeyDown( SDL_SCANCODE_5 ) && keyboard_handler_.isShiftDown() )
   {
      previous             = Key_Press{ SDL_SCANCODE_5, true };
      computer_.keyboard() = 37;                                  // %
   }

   else if ( ( previous.key != SDL_SCANCODE_6 || previous.shift != false ) && 
               keyboard_handler_.isKeyDown( SDL_SCANCODE_6 ) && keyboard_handler_.isShiftUp() )
   {
      previous             = Key_Press{ SDL_SCANCODE_6, false };
      computer_.keyboard() = 54;                                  // 6
   }

   else if ( ( previous.key != SDL_SCANCODE_6 || previous.shift != true ) &&
               keyboard_handler_.isKeyDown( SDL_SCANCODE_6 ) && keyboard_handler_.isShiftDown() )
   {
      previous             = Key_Press{ SDL_SCANCODE_6, true };
      computer_.keyboard() = 94;                                  // ^
   }

   else if ( ( previous.key != SDL_SCANCODE_7 || previous.shift != false ) && 
               keyboard_handler_.isKeyDown( SDL_SCANCODE_7 ) && keyboard_handler_.isShiftUp() )
   {
      previous             = Key_Press{ SDL_SCANCODE_7, false };
      computer_.keyboard() = 55;                                  // 7
   }

   else if ( ( previous.key != SDL_SCANCODE_7 || previous.shift != true ) &&
               keyboard_handler_.isKeyDown( SDL_SCANCODE_7 ) && keyboard_handler_.isShiftDown() )
   {
      previous             = Key_Press{ SDL_SCANCODE_7, true };
      computer_.keyboard() = 38;                                  // &
   }

   else if ( ( previous.key != SDL_SCANCODE_8 || previous.shift != false ) && 
               keyboard_handler_.isKeyDown( SDL_SCANCODE_8 ) && keyboard_handler_.isShiftUp() )
   {
      previous             = Key_Press{ SDL_SCANCODE_8, false };
      computer_.keyboard() = 56;                                  // 8
   }

   else if ( ( previous.key != SDL_SCANCODE_8 || previous.shift != true ) &&
               keyboard_handler_.isKeyDown( SDL_SCANCODE_8 ) && keyboard_handler_.isShiftDown() )
   {
      previous             = Key_Press{ SDL_SCANCODE_8, true };
      computer_.keyboard() = 42;                                  // *
   }

   else if ( ( previous.key != SDL_SCANCODE_9 || previous.shift != false ) && 
               keyboard_handler_.isKeyDown( SDL_SCANCODE_9 ) && keyboard_handler_.isShiftUp() )
   {
      previous             = Key_Press{ SDL_SCANCODE_9, false };
      computer_.keyboard() = 57;                                  // 9
   }

   else if ( ( previous.key != SDL_SCANCODE_9 || previous.shift != true ) &&
               keyboard_handler_.isKeyDown( SDL_SCANCODE_9 ) && keyboard_handler_.isShiftDown() )
   {
      previous             = Key_Press{ SDL_SCANCODE_9, true };
      computer_.keyboard() = 40;                                  // (
   }

   else if ( ( previous.key != SDL_SCANCODE_0 || previous.shift != false ) && 
               keyboard_handler_.isKeyDown( SDL_SCANCODE_0 ) && keyboard_handler_.isShiftUp() )
   {
      previous             = Key_Press{ SDL_SCANCODE_0, false };
      computer_.keyboard() = 48;                                  // 0
   }

   else if ( ( previous.key != SDL_SCANCODE_0 || previous.shift != true ) &&
               keyboard_handler_.isKeyDown( SDL_SCANCODE_0 ) && keyboard_handler_.isShiftDown() )
   {
      previous             = Key_Press{ SDL_SCANCODE_0, true };
      computer_.keyboard() = 41;                                  // )
   }

   else if ( ( previous.key != SDL_SCANCODE_MINUS || previous.shift != false ) && 
               keyboard_handler_.isKeyDown( SDL_SCANCODE_MINUS ) && keyboard_handler_.isShiftUp() )
   {
      previous             = Key_Press{ SDL_SCANCODE_MINUS, false };
      computer_.keyboard() = 45;                                  // -
   }

   else if ( ( previous.key != SDL_SCANCODE_MINUS || previous.shift != true ) &&
               keyboard_handler_.isKeyDown( SDL_SCANCODE_MINUS ) && keyboard_handler_.isShiftDown() )
   {
      previous             = Key_Press{ SDL_SCANCODE_MINUS, true };
      computer_.keyboard() = 95;                                  // _
   }

   else if ( ( previous.key != SDL_SCANCODE_EQUALS || previous.shift != false ) && 
               keyboard_handler_.isKeyDown( SDL_SCANCODE_EQUALS ) && keyboard_handler_.isShiftUp() )
   {
      previous             = Key_Press{ SDL_SCANCODE_EQUALS, false };
      computer_.keyboard() = 61;                                  // =
   }

   else if ( ( previous.key != SDL_SCANCODE_EQUALS || previous.shift != true ) &&
               keyboard_handler_.isKeyDown( SDL_SCANCODE_EQUALS ) && keyboard_handler_.isShiftDown() )
   {
      previous             = Key_Press{ SDL_SCANCODE_EQUALS, true };
      computer_.keyboard() = 43;                                  // +
   }

   else if ( ( previous.key != SDL_SCANCODE_SEMICOLON || previous.shift != false ) && 
               keyboard_handler_.isKeyDown( SDL_SCANCODE_SEMICOLON ) && keyboard_handler_.isShiftUp() )
   {
      previous             = Key_Press{ SDL_SCANCODE_SEMICOLON, false };
      computer_.keyboard() = 59;                                  // ;
   }

   else if ( ( previous.key != SDL_SCANCODE_SEMICOLON || previous.shift != true ) &&
               keyboard_handler_.isKeyDown( SDL_SCANCODE_SEMICOLON ) && keyboard_handler_.isShiftDown() )
   {
      previous             = Key_Press{ SDL_SCANCODE_SEMICOLON, true };
      computer_.keyboard() = 58;                                  // :
   }

   else if ( ( previous.key != SDL_SCANCODE_COMMA || previous.shift != false ) && 
               keyboard_handler_.isKeyDown( SDL_SCANCODE_COMMA ) && keyboard_handler_.isShiftUp() )
   {
      previous             = Key_Press{ SDL_SCANCODE_COMMA, false };
      computer_.keyboard() = 44;                                  // ,
   }

   else if ( ( previous.key != SDL_SCANCODE_COMMA || previous.shift != true ) &&
               keyboard_handler_.isKeyDown( SDL_SCANCODE_COMMA ) && keyboard_handler_.isShiftDown() )
   {
      previous             = Key_Press{ SDL_SCANCODE_COMMA, true };
      computer_.keyboard() = 60;                                  // <
   }
   
   else if ( ( previous.key != SDL_SCANCODE_PERIOD || previous.shift != false ) && 
               keyboard_handler_.isKeyDown( SDL_SCANCODE_PERIOD ) && keyboard_handler_.isShiftUp() )
   {
      previous             = Key_Press{ SDL_SCANCODE_PERIOD, false };
      computer_.keyboard() = 46;                                  // .
   }

   else if ( ( previous.key != SDL_SCANCODE_PERIOD || previous.shift != true ) &&
               keyboard_handler_.isKeyDown( SDL_SCANCODE_PERIOD ) && keyboard_handler_.isShiftDown() )
   {
      previous             = Key_Press{ SDL_SCANCODE_PERIOD, true };
      computer_.keyboard() = 62;                                  // >
   }

   else if ( ( previous.key != SDL_SCANCODE_SLASH || previous.shift != false ) && 
               keyboard_handler_.isKeyDown( SDL_SCANCODE_SLASH ) && keyboard_handler_.isShiftUp() )
   {
      previous             = Key_Press{ SDL_SCANCODE_SLASH, false };
      computer_.keyboard() = 47;                                  // /
   }

   else if ( ( previous.key != SDL_SCANCODE_SLASH || previous.shift != true ) &&
               keyboard_handler_.isKeyDown( SDL_SCANCODE_SLASH ) && keyboard_handler_.isShiftDown() )
   {
      previous             = Key_Press{ SDL_SCANCODE_SLASH, true };
      computer_.keyboard() = 63;                                  // ?
   }

   else if ( ( previous.key != SDL_SCANCODE_APOSTROPHE || previous.shift != false ) && 
               keyboard_handler_.isKeyDown( SDL_SCANCODE_APOSTROPHE ) && keyboard_handler_.isShiftUp() )
   {
      previous             = Key_Press{ SDL_SCANCODE_APOSTROPHE, false };
      computer_.keyboard() = 39;                                  // '
   }

   else if ( ( previous.key != SDL_SCANCODE_APOSTROPHE || previous.shift != true ) &&
               keyboard_handler_.isKeyDown( SDL_SCANCODE_APOSTROPHE ) && keyboard_handler_.isShiftDown() )
   {
      previous             = Key_Press{ SDL_SCANCODE_APOSTROPHE, true };
      computer_.keyboard() = 34;                                  // "
   }

   else if ( ( previous.key != SDL_SCANCODE_GRAVE || previous.shift != false ) && 
               keyboard_handler_.isKeyDown( SDL_SCANCODE_GRAVE ) && keyboard_handler_.isShiftUp() )
   {
      previous             = Key_Press{ SDL_SCANCODE_GRAVE, false };
      computer_.keyboard() = 96;                                  // `
   }

   else if ( ( previous.key != SDL_SCANCODE_GRAVE || previous.shift != true ) &&
               keyboard_handler_.isKeyDown( SDL_SCANCODE_GRAVE ) && keyboard_handler_.isShiftDown() )
   {
      previous             = Key_Press{ SDL_SCANCODE_GRAVE, true };
      computer_.keyboard() = 126;                                  // ~
   }

   else if ( ( previous.key != SDL_SCANCODE_LEFTBRACKET || previous.shift != false ) && 
               keyboard_handler_.isKeyDown( SDL_SCANCODE_LEFTBRACKET ) && keyboard_handler_.isShiftUp() )
   {
      previous             = Key_Press{ SDL_SCANCODE_LEFTBRACKET, false };
      computer_.keyboard() = 91;                                  // [
   }

   else if ( ( previous.key != SDL_SCANCODE_LEFTBRACKET || previous.shift != true ) &&
               keyboard_handler_.isKeyDown( SDL_SCANCODE_LEFTBRACKET ) && keyboard_handler_.isShiftDown() )
   {
      previous             = Key_Press{ SDL_SCANCODE_LEFTBRACKET, true };
      computer_.keyboard() = 123;                                  // {
   }

   else if ( ( previous.key != SDL_SCANCODE_RIGHTBRACKET || previous.shift != false ) && 
               keyboard_handler_.isKeyDown( SDL_SCANCODE_RIGHTBRACKET ) && keyboard_handler_.isShiftUp() )
   {
      previous             = Key_Press{ SDL_SCANCODE_RIGHTBRACKET, false };
      computer_.keyboard() = 93;                                  // ]
   }

   else if ( ( previous.key != SDL_SCANCODE_RIGHTBRACKET || previous.shift != true ) &&
               keyboard_handler_.isKeyDown( SDL_SCANCODE_RIGHTBRACKET ) && keyboard_handler_.isShiftDown() )
   {
      previous             = Key_Press{ SDL_SCANCODE_RIGHTBRACKET, true };
      computer_.keyboard() = 125;                                  // }
   }

   else if ( ( previous.key != SDL_SCANCODE_BACKSLASH || previous.shift != false ) && 
               keyboard_handler_.isKeyDown( SDL_SCANCODE_BACKSLASH ) && keyboard_handler_.isShiftUp() )
   {
      previous             = Key_Press{ SDL_SCANCODE_BACKSLASH, false };
      computer_.keyboard() = 92;                                  // '\'
   }

   else if ( ( previous.key != SDL_SCANCODE_BACKSLASH || previous.shift != true ) &&
               keyboard_handler_.isKeyDown( SDL_SCANCODE_BACKSLASH ) && keyboard_handler_.isShiftDown() )
   {
      previous             = Key_Press{ SDL_SCANCODE_BACKSLASH, true };
      computer_.keyboard() = 124;                                  // |
   }


   // Alphabetic ----------------------------------------------------------------------------------

   else if ( previous.key != SDL_SCANCODE_A && keyboard_handler_.isKeyDown( SDL_SCANCODE_A ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_A, false };
      computer_.keyboard() = 65;                   // A
   }

   else if ( previous.key != SDL_SCANCODE_B && keyboard_handler_.isKeyDown( SDL_SCANCODE_B ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_B, false };
      computer_.keyboard() = 66;                   // B
   }

   else if ( previous.key != SDL_SCANCODE_C && keyboard_handler_.isKeyDown( SDL_SCANCODE_C ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_C, false };
      computer_.keyboard() = 67;                   // C
   }

   else if ( previous.key != SDL_SCANCODE_D && keyboard_handler_.isKeyDown( SDL_SCANCODE_D ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_D, false };
      computer_.keyboard() = 68;                   // D
   }

   else if ( previous.key != SDL_SCANCODE_E && keyboard_handler_.isKeyDown( SDL_SCANCODE_E ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_E, false };
      computer_.keyboard() = 69;                   // E
   }

   else if ( previous.key != SDL_SCANCODE_F && keyboard_handler_.isKeyDown( SDL_SCANCODE_F ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_F, false };
      computer_.keyboard() = 70;                   // F
   }

   else if ( previous.key != SDL_SCANCODE_G && keyboard_handler_.isKeyDown( SDL_SCANCODE_G ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_G, false };
      computer_.keyboard() = 71;                   // G
   }

   else if ( previous.key != SDL_SCANCODE_H && keyboard_handler_.isKeyDown( SDL_SCANCODE_H ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_H, false };
      computer_.keyboard() = 72;                   // H
   }

   else if ( previous.key != SDL_SCANCODE_I && keyboard_handler_.isKeyDown( SDL_SCANCODE_I ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_I, false };
      computer_.keyboard() = 73;                   // I
   }

   else if ( previous.key != SDL_SCANCODE_J && keyboard_handler_.isKeyDown( SDL_SCANCODE_J ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_J, false };
      computer_.keyboard() = 74;                   // J
   }

   else if ( previous.key != SDL_SCANCODE_K && keyboard_handler_.isKeyDown( SDL_SCANCODE_K ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_K, false };
      computer_.keyboard() = 75;                   // K
   }

   else if ( previous.key != SDL_SCANCODE_L && keyboard_handler_.isKeyDown( SDL_SCANCODE_L ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_L, false };
      computer_.keyboard() = 76;                   // L
   }

   else if ( previous.key != SDL_SCANCODE_M && keyboard_handler_.isKeyDown( SDL_SCANCODE_M ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_M, false };
      computer_.keyboard() = 77;                   // M
   }

   else if ( previous.key != SDL_SCANCODE_N && keyboard_handler_.isKeyDown( SDL_SCANCODE_N ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_N, false };
      computer_.keyboard() = 78;                   // N
   }

   else if ( previous.key != SDL_SCANCODE_O && keyboard_handler_.isKeyDown( SDL_SCANCODE_O ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_O, false };
      computer_.keyboard() = 79;                   // O
   }

   else if ( previous.key != SDL_SCANCODE_P && keyboard_handler_.isKeyDown( SDL_SCANCODE_P ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_P, false };
      computer_.keyboard() = 80;                   // P
   }

   else if ( previous.key != SDL_SCANCODE_Q && keyboard_handler_.isKeyDown( SDL_SCANCODE_Q ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_Q, false };
      computer_.keyboard() = 81;                   // Q
   }

   else if ( previous.key != SDL_SCANCODE_R && keyboard_handler_.isKeyDown( SDL_SCANCODE_R ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_R, false };
      computer_.keyboard() = 82;                   // R
   }

   else if ( previous.key != SDL_SCANCODE_S && keyboard_handler_.isKeyDown( SDL_SCANCODE_S ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_S, false };
      computer_.keyboard() = 83;                   // S
   }

   else if ( previous.key != SDL_SCANCODE_T && keyboard_handler_.isKeyDown( SDL_SCANCODE_T ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_T, false };
      computer_.keyboard() = 84;                   // T
   }

   else if ( previous.key != SDL_SCANCODE_U && keyboard_handler_.isKeyDown( SDL_SCANCODE_U ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_U, false };
      computer_.keyboard() = 85;                   // U
   }

   else if ( previous.key != SDL_SCANCODE_V && keyboard_handler_.isKeyDown( SDL_SCANCODE_V ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_V, false };
      computer_.keyboard() = 86;                   // V
   }

   else if ( previous.key != SDL_SCANCODE_W && keyboard_handler_.isKeyDown( SDL_SCANCODE_W ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_W, false };
      computer_.keyboard() = 87;                   // W
   }

   else if ( previous.key != SDL_SCANCODE_X && keyboard_handler_.isKeyDown( SDL_SCANCODE_X ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_X, false };
      computer_.keyboard() = 88;                   // X
   }

   else if ( previous.key != SDL_SCANCODE_Y && keyboard_handler_.isKeyDown( SDL_SCANCODE_Y ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_Y, false };
      computer_.keyboard() = 89;                   // Y
   }

   else if ( previous.key != SDL_SCANCODE_Z && keyboard_handler_.isKeyDown( SDL_SCANCODE_Z ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_Z, false };
      computer_.keyboard() = 90;                   // Z
   }

   

   // --------------
   else if ( previous.key != SDL_SCANCODE_RETURN && keyboard_handler_.isKeyDown( SDL_SCANCODE_RETURN ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_RETURN, false };
      computer_.keyboard() = 128;                   // newLine
   }

   else if ( previous.key != SDL_SCANCODE_BACKSPACE && keyboard_handler_.isKeyDown( SDL_SCANCODE_BACKSPACE ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_BACKSPACE, false };
      computer_.keyboard() = 129;                   // backSpace
   }

   else if ( previous.key != SDL_SCANCODE_LEFT && keyboard_handler_.isKeyDown( SDL_SCANCODE_LEFT ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_LEFT, false };
      computer_.keyboard() = 130;                   // left
   }
   
   else if ( previous.key != SDL_SCANCODE_UP && keyboard_handler_.isKeyDown( SDL_SCANCODE_UP ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_UP, false };
      computer_.keyboard() = 131;                   // up
   }

   else if ( previous.key != SDL_SCANCODE_RIGHT && keyboard_handler_.isKeyDown( SDL_SCANCODE_RIGHT ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_RIGHT, false };
      computer_.keyboard() = 132;                   // right
   }

   else if ( previous.key != SDL_SCANCODE_DOWN && keyboard_handler_.isKeyDown( SDL_SCANCODE_DOWN ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_DOWN, false };
      computer_.keyboard() = 133;                   // down
   }

   else if ( previous.key != SDL_SCANCODE_HOME && keyboard_handler_.isKeyDown( SDL_SCANCODE_HOME ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_HOME, false };
      computer_.keyboard() = 134;                   // home
   }

   else if ( previous.key != SDL_SCANCODE_END && keyboard_handler_.isKeyDown( SDL_SCANCODE_END ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_END, false };
      computer_.keyboard() = 135;                   // home
   }

   else if ( previous.key != SDL_SCANCODE_PAGEUP && keyboard_handler_.isKeyDown( SDL_SCANCODE_PAGEUP ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_PAGEUP, false };
      computer_.keyboard() = 136;                   // pageUp
   }   

   else if ( previous.key != SDL_SCANCODE_PAGEDOWN && keyboard_handler_.isKeyDown( SDL_SCANCODE_PAGEDOWN ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_PAGEDOWN, false };
      computer_.keyboard() = 137;                   // pageDown
   }   

   else if ( previous.key != SDL_SCANCODE_INSERT && keyboard_handler_.isKeyDown( SDL_SCANCODE_INSERT ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_INSERT, false };
      computer_.keyboard() = 138;                   // insert
   }   

   else if ( previous.key != SDL_SCANCODE_DELETE && keyboard_handler_.isKeyDown( SDL_SCANCODE_DELETE ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_DELETE, false };
      computer_.keyboard() = 139;                   // insert
   }   

   else if ( previous.key != SDL_SCANCODE_ESCAPE && keyboard_handler_.isKeyDown( SDL_SCANCODE_ESCAPE ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_ESCAPE, false };
      computer_.keyboard() = 140;                   // ESC
   }

   else if ( previous.key != SDL_SCANCODE_F1 && keyboard_handler_.isKeyDown( SDL_SCANCODE_F1 ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_F1, false };
      computer_.keyboard() = 141;                   // F1
   }
   
   else if ( previous.key != SDL_SCANCODE_F2 && keyboard_handler_.isKeyDown( SDL_SCANCODE_F2 ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_F2, false };
      computer_.keyboard() = 142;                   // F2
   }

   else if ( previous.key != SDL_SCANCODE_F3 && keyboard_handler_.isKeyDown( SDL_SCANCODE_F3 ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_F3, false };
      computer_.keyboard() = 143;                   // F3
   }

   else if ( previous.key != SDL_SCANCODE_F4 && keyboard_handler_.isKeyDown( SDL_SCANCODE_F4 ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_F4, false };
      computer_.keyboard() = 144;                   // F4
   }

   else if ( previous.key != SDL_SCANCODE_F5 && keyboard_handler_.isKeyDown( SDL_SCANCODE_F5 ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_F5, false };
      computer_.keyboard() = 145;                   // F5
   }

   else if ( previous.key != SDL_SCANCODE_F6 && keyboard_handler_.isKeyDown( SDL_SCANCODE_F6 ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_F6, false };
      computer_.keyboard() = 146;                   // F6
   }

   else if ( previous.key != SDL_SCANCODE_F7 && keyboard_handler_.isKeyDown( SDL_SCANCODE_F7 ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_F7, false };
      computer_.keyboard() = 147;                   // F7
   }

   else if ( previous.key != SDL_SCANCODE_F8 && keyboard_handler_.isKeyDown( SDL_SCANCODE_F8 ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_F8, false };
      computer_.keyboard() = 148;                   // F8
   }

   else if ( previous.key != SDL_SCANCODE_F9 && keyboard_handler_.isKeyDown( SDL_SCANCODE_F9 ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_F9, false };
      computer_.keyboard() = 149;                   // F9
   }

   else if ( previous.key != SDL_SCANCODE_F10 && keyboard_handler_.isKeyDown( SDL_SCANCODE_F10 ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_F10, false };
      computer_.keyboard() = 150;                   // F10
   }

   else if ( previous.key != SDL_SCANCODE_F11 && keyboard_handler_.isKeyDown( SDL_SCANCODE_F11 ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_F11, false };
      computer_.keyboard() = 151;                   // F11
   }

   else if ( previous.key != SDL_SCANCODE_F12 && keyboard_handler_.isKeyDown( SDL_SCANCODE_F12 ) )
   {
      previous             = Key_Press{ SDL_SCANCODE_F12, false };
      computer_.keyboard() = 152;                   // F12
   }
   else
   {
      // no key press
      previous = Key_Press{};
   }
}


auto 
Hack::Emulator::open_file( std::string const& path )  -> void
{
   auto data = [&path] 
   {
      if ( path.ends_with( ".hack" ) )
      {
         return Hack::EMULATOR::Utils::open_hack_file( path );
      }
      else if ( path.ends_with( ".asm" ) )
      {
         return Hack::EMULATOR::Utils::open_asm_file( path );
      }

      throw Hack::EMULATOR::Utils::unsupported_filetype_error( "Could not open file: " + path, nullptr );
      
      return std::vector<std::uint16_t>();
   }();

   computer_.clear();
   computer_.load_rom( data );
}


auto 
Hack::Emulator::update_Hack_Computer()       -> void
{
   // ImGui operates at the monitor refresh rate with vsync enabled
   //std::cerr << "FPS: " << ImGui::GetIO().Framerate << '\n'; 
   if ( play_ || step_ )
   {
      if ( computer_.pc() > Hack::Computer::ROM_SIZE )
      {
         play_ = false;
         step_ = false;
         computer_.pc() = 0;
      }

      if ( play_ )
      {
         auto const FPS = ImGui::GetIO().Framerate;

         if ( speed_ < FPS )
         {
            static auto count = 0;
            static auto delay = static_cast<int>( FPS / speed_ );

            if ( count < delay )
            {
               delay = static_cast<int>( FPS / speed_ );
               ++count;
            }
            else
            {
               computer_.execute();
               count = 0;
            }
         }
         else
         {
            auto const instructions = static_cast<int>( speed_ / FPS );
            
            for ( auto cnt = 0; cnt < instructions; ++cnt )
            {
               computer_.execute();
            }
         }
      }
      else if ( step_ )
      {
         computer_.execute();
         step_ = false;
      }
   }
}


auto 
Hack::Emulator::update_GUI_interface() -> void
{
   static constexpr auto window_flags = ImGuiWindowFlags_NoCollapse       | ImGuiWindowFlags_NoResize    | ImGuiWindowFlags_MenuBar    |
                                        ImGuiWindowFlags_NoMove           | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar |
                                        ImGuiWindowFlags_NoSavedSettings;

   ImGui::SetNextWindowPos( ImVec2{ 0, 0 }, ImGuiCond_Always );

   static int window_width;
   static int window_height;

   SDL_GetWindowSize( window_, &window_width, &window_height );
   ImGui::SetNextWindowSize( ImVec2{ static_cast<float>( window_width ),static_cast<float>( window_height )}, ImGuiCond_Always );

   with_Window( "Main", nullptr, window_flags )
   {
     
         main_window();
         displays_errors();
         
         if ( open_new_file_ )
         {
            open_new_file_ = false;
            open_file( current_file_ );
         }
     
      
   }
}


auto 
Hack::Emulator::main_window()  -> void
{
   menu_bar();

   // Main Command Window ---------------------------------------------------------------------------
   auto  const options = command_GUI();

   // ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 15.0f );
   set_StyleVar( ImGuiStyleVar_ChildRounding, 15.0f );

   // Memory Displays --------------------------------------------------------------------------
   auto const child_height = ImGui::GetContentRegionAvail().y - 75;

   with_Child( "##ROM Display", ImVec2( 225 , child_height), ImGuiChildFlags_Border  )
   {
      ROM_GUI( play_ || step_  || options.track_pc );
   }

   ImGui::SameLine();

   with_Child( "##RAM Display", ImVec2( 225 , child_height), ImGuiChildFlags_Border  )
   {
      RAM_GUI();
   }

   ImGui::SameLine();

   with_Child( "##Screen Display", ImVec2( 225 , child_height), ImGuiChildFlags_Border )
   {  
      Screen_GUI();
   }

   // Screen Area & CPU Display ----------------------------------------------------------------
   ImGui::SameLine();

   with_Group
   {
      with_Child( "##Hack Computer Screen", ImVec2( ImGui::GetContentRegionAvail().x , 300 ), ImGuiChildFlags_Border )
      {  
         with_StyleVar( ImGuiStyleVar_SeparatorTextAlign, ImVec2{ 0.5, 0.5 } )
            ImGui::SeparatorText( "Hack Computer Screen" );
        
         ImGui::NewLine();
         auto const indent = ( ImGui::GetContentRegionAvail().x - screen_texture_.width ) / 2 + ImGui::GetCursorPosX();
         ImGui::SameLine( indent );

         ImGui::Image( (void*) screen_texture_.texture(), ImVec2( screen_texture_.width, screen_texture_.height ) );
      }

      with_Child( "##CPU", ImVec2( ImGui::GetContentRegionAvail().x , ImGui::GetContentRegionAvail().y - 75), ImGuiChildFlags_Border )
      {
         
      }
   }  

   // Status Bar -------------------------------------------------------------------------------
   with_Child( "##Status Bar", ImVec2( ImGui::GetContentRegionAvail().x , ImGui::GetContentRegionAvail().y  ), ImGuiChildFlags_Border )
   {
      ImGui::SeparatorText( "Status" );
   }
   
}


auto
Hack::Emulator::menu_bar()                   -> void
{
   with_MenuBar
   {
      with_Menu( "File" )
      {  
         if ( ImGui::MenuItem(" Open" ) ) 
         {
            auto config   = IGFD::FileDialogConfig();
            config.flags  = ImGuiFileDialogFlags_Modal;
            config.path   = "../";
            ImGuiFileDialog::Instance()->OpenDialog( "ChooseFileDlgKey", "Open File", ".hack,.asm,.*", config );
         }
      }

      with_Menu( "Edit" )
      {
         if ( ImGui::MenuItem( "Undo", "CTRL+Z" ) ) {}
         if ( ImGui::MenuItem( "Redo", "CTRL+Y", false, false ) ) {}  // Disabled item
         ImGui::Separator();
         if ( ImGui::MenuItem("Cut", "CTRL+X" ) ) {}
         if ( ImGui::MenuItem("Copy", "CTRL+C" ) ) {}
         if ( ImGui::MenuItem("Paste", "CTRL+V") ) {}
      }

      with_Menu( "Theme" )
      {  
         if ( ImGui::MenuItem( "Dark" ) )  { ImGui::StyleColorsDark(); }
         if ( ImGui::MenuItem( "Light" ) ) { ImGui::StyleColorsLight(); }
      }
   }
}


auto
Hack::Emulator::command_GUI() -> main_options
{
   auto track_pc = false;

   with_Child( "##Menu Bar", ImVec2( ImGui::GetContentRegionAvail().x , 75.0 ), ImGuiChildFlags_Border ) 
   {
      // open File Dialog
      if ( ImGui::Button( "Open File" ) )
      {
            auto config   = IGFD::FileDialogConfig();
            config.flags  = ImGuiFileDialogFlags_Modal;
            config.path   = "../";
            ImGuiFileDialog::Instance()->OpenDialog( "ChooseFileDlgKey", "Open File", ".hack,.asm,.*", config );
      }
      
      if ( ImGuiFileDialog::Instance()->Display( "ChooseFileDlgKey", ImGuiWindowFlags_NoCollapse, ImVec2{ 512, 256 } ) )
      {
         if ( ImGuiFileDialog::Instance()->IsOk() )
         { 
            // action if OK
            current_file_  = ImGuiFileDialog::Instance()->GetFilePathName();
            open_new_file_ = true;
         }
        
         // close
         ImGuiFileDialog::Instance()->Close();
      }

      ImGui::SameLine();
      if ( ImGui::Button( "Step" ) )
      {
         step_  = true;
      }

      ImGui::SameLine();
      if ( ImGui::Button( "Play" ) )
      {
            play_ = true;
      }

      ImGui::SameLine();
      if ( ImGui::Button( "Stop" ) )
      {
         play_ = false;
      }

      ImGui::SameLine();

      if ( ImGui::Button( "Restart" ) )
      {
            computer_.reset();
            play_    = false;
            step_    = false;
            track_pc = true;
      }

      ImGui::SameLine();
      ImGui::TextUnformatted( "  Speed:" );
      ImGui::SameLine();
      ImGui::PushItemWidth( 200 );
      ImGui::SliderFloat( "##Speed", &speed_, 0.3F, 5'000'000.0F, "%.1f", ImGuiSliderFlags_Logarithmic );
      ImGui::PopItemWidth();

   }

   return { track_pc, RAMFormat::DECIMAL };
}


auto
Hack::Emulator::ROM_GUI( bool highlight_pc ) -> void
{
   static constexpr auto rom_size = Hack::Computer::ROM_SIZE;

   static auto const& pc      = computer_.pc();
   static auto enable_track   = false;
   static auto track_item     = std::uint16_t{ 0 };
   static auto display_format = 0;

   if ( ImGui::Button( "Find" ) )
   {
      enable_track = true;
   } 

   ImGui::SameLine();
   ImGui::SetNextItemWidth( 55.0 );
   ImGui::InputScalar( "##address", ImGuiDataType_U16, &track_item );
   track_item = ( track_item > rom_size ) ? rom_size - 1 : track_item ;

   ImGui::SameLine();
   if( ImGui::Button( "Clear" ) )
   {
      ImGui::OpenPopup( "Clear ROM?" );
   }
   
   // "Clear ROM?" --------------------------------------------------------------------------------
   with_StyleVar( ImGuiStyleVar_WindowTitleAlign, ImVec2{ 0.5, 0.5 } )
   with_StyleVar( ImGuiStyleVar_PopupRounding, 10.0 )
   {
      auto clear_ROM_popup_open = true;

      with_PopupModal( "Clear ROM?", &clear_ROM_popup_open ) 
      {
         ImGui::TextUnformatted( "This action cannot be undone" );
         ImGui::Spacing();
         ImGui::Indent( 30.0 );
         if ( ImGui::Button( "Confirm" ) )
         {
            computer_.clear_rom();
            ImGui::CloseCurrentPopup();
         }
      
         ImGui::SameLine( 120.0 ); 
         
         if ( ImGui::Button( "Cancel" ) )
         {
            ImGui::CloseCurrentPopup();
         }
      }
   }

   // ---------------------------------------------------------------------------------------------

   ImGui::SameLine();
   ImGui::SetNextItemWidth( 50.0 );
   ImGui::Combo( "##Display", &display_format, "ASM\0DEC\0HEX\0BIN\0\0" );
   ImGui::SeparatorText( "ROM" );

   with_StyleVar( ImGuiStyleVar_ChildRounding, 15.0f )
   with_Child("RomChild", ImVec2( ImGui::GetContentRegionAvail().x , ImGui::GetContentRegionAvail().y ), ImGuiChildFlags_Border ) 
   {
      auto clipper = ImGuiListClipper();
      clipper.Begin( rom_size );

      if ( enable_track )
         clipper.IncludeItemByIndex( track_item );
      else
         clipper.IncludeItemByIndex( pc );
      
      while( clipper.Step() )
      {
         for ( auto idx = clipper.DisplayStart; idx < clipper.DisplayEnd; ++idx )
         {
            with_ID( idx )
            {
               ROM_Display( static_cast<ROMFormat>( display_format ), idx );

               if ( enable_track && idx == track_item )
               {
                  ImGui::SetScrollHereY( 0 );
                  enable_track = false;
               }
               else if ( highlight_pc && idx == pc )
               {
                  ImGui::SetScrollHereY( 0.25 );
               }
            }
         }
      } 
   }
}


auto
Hack::Emulator::ROM_Display( ROMFormat const fmt, int const idx ) -> void
{  
   static auto& rom      = computer_.ROM();
   static auto const& pc = computer_.pc();

   auto& instruction = rom[static_cast<Hack::Computer::ROM_t::size_type>( idx )];

   switch ( fmt )
   {
   case ROMFormat::ASM:
   {
      auto asm_opt         = disasmblr_.disassemble( instruction );
      auto asm_instruction = std::string( "---" );

      if ( asm_opt )
         asm_instruction = *asm_opt;
      
      if ( idx == pc )
      {
         with_StyleColor( ImGuiCol_Text, IM_COL32( 230, 255, 0, 255 ) )
         {
            ImGui::AlignTextToFramePadding();
            ImGui::Text( "%d", idx );
            ImGui::SameLine( 60 );
            ImGui::InputText( "rom", &asm_instruction );
         }
      }
      else
      {
         ImGui::AlignTextToFramePadding();
         ImGui::Text( "%d", idx );
         ImGui::SameLine( 60 );
         ImGui::InputText( "rom", &asm_instruction );
      }

      if ( asm_instruction ==  "---" )
         return;

      if ( asm_opt && *asm_opt == asm_instruction )
         return;
      
      auto const binary_opt = assembler_.assemble( asm_instruction );
      
      if( !binary_opt )
         return;

      auto const value_opt = Hack::Utils::binary_to_uint16( *binary_opt );
      if ( value_opt )
      {
         instruction = *value_opt;
      }  
      
      return;
   }
   
   case ROMFormat::DECIMAL:
   {
      auto value = Hack::Utils::unsigned_to_signed_16( instruction );

      if ( idx == pc )
      {
         with_StyleColor( ImGuiCol_Text, IM_COL32( 230, 255, 0, 255 ) )
         {
            ImGui::AlignTextToFramePadding();
            ImGui::Text( "%d", idx );
            ImGui::SameLine( 60 );
            ImGui::InputScalar( "##rom", ImGuiDataType_S16, &value );
         }        
      }
      else
      {
         ImGui::AlignTextToFramePadding();
         ImGui::Text( "%d", idx );
         ImGui::SameLine( 60 );
         ImGui::InputScalar( "##rom", ImGuiDataType_S16, &value );
      }

      if ( value != instruction )
      {
         instruction = Hack::Utils::signed_to_unsigned_16( value );
      }
      return;
   }
   
   case ROMFormat::HEX:
   {
      auto value = instruction;

      if ( idx == pc )
      {
         with_StyleColor( ImGuiCol_Text, IM_COL32( 230, 255, 0, 255 ) )
         {
            ImGui::AlignTextToFramePadding();
            ImGui::Text( "%d", idx );
            ImGui::SameLine( 60 );
            ImGui::InputScalar( "##rom", ImGuiDataType_U16, &value, nullptr, nullptr, "%04X" );
         }
      }
      else
      {
         ImGui::AlignTextToFramePadding();
         ImGui::Text( "%d", idx );
         ImGui::SameLine( 60 );
         ImGui::InputScalar( "##rom", ImGuiDataType_U16, &value, nullptr, nullptr, "%04X" );
      }

      if ( value != instruction )
      {
         instruction = value;
      }
      return;
   }

   case ROMFormat::BINARY:
   {
      auto binary = Hack::Utils::to_binary16_string( instruction );

      if ( idx == pc )
      {
         with_StyleColor( ImGuiCol_Text, IM_COL32( 230, 255, 0, 255 ) )
         {
            ImGui::AlignTextToFramePadding();
            ImGui::Text( "%d", idx );
            ImGui::SameLine( 60 );
            ImGui::InputText( "rom", &binary );
         }
      }
      else
      {
         ImGui::AlignTextToFramePadding();
         ImGui::Text( "%d", idx );
         ImGui::SameLine( 60 );
         ImGui::InputText( "rom", &binary );
      }

      if ( auto const value = Hack::Utils::binary_to_uint16( binary ); value )
      {
         if ( *value != instruction )
         {
            instruction = *value;
         }
      }
      return;
   }

   default:
   {
      std::cerr << "Unkown Format in ROM Display\n";
      break;
   }

   }  // switch( fmt )
}


auto
Hack::Emulator::RAM_GUI( ) -> void
{
   static constexpr auto ram_size = Hack::Computer::RAM_SIZE;

   static auto enable_track   = false;
   static auto track_item     = std::uint16_t{ 0 };
   static auto display_format = 0;

   if ( ImGui::Button( "Find" ) )
   {
      enable_track = true;
   } 

   ImGui::SameLine();
   ImGui::SetNextItemWidth( 55.0 );
   ImGui::InputScalar( "##address", ImGuiDataType_U16, &track_item );
   track_item = ( track_item > ram_size ) ? ram_size - 1 : track_item ;

   ImGui::SameLine();
   if( ImGui::Button( "Clear" ) )
   {
      ImGui::OpenPopup( "Clear RAM?" );
   }

   // "Clear RAM?"" -------------------------------------------------------------------------------
   with_StyleVar( ImGuiStyleVar_PopupRounding, 10.0 )
   with_StyleVar( ImGuiStyleVar_WindowTitleAlign, ImVec2{ 0.5, 0.5 } )
   {
      auto clear_RAM_popup_open = true;
      with_PopupModal( "Clear RAM?", &clear_RAM_popup_open )
      {
         ImGui::TextUnformatted( "This action cannot be undone" );
         ImGui::Spacing();
         ImGui::Indent( 30.0 );
         if ( ImGui::Button( "Confirm" ) )
         {
            computer_.clear_ram();
            ImGui::CloseCurrentPopup();
         }
      
         ImGui::SameLine( 120.0 ); 
         
         if ( ImGui::Button( "Cancel" ) )
         {
            ImGui::CloseCurrentPopup();
         }
      }
   }
   // ---------------------------------------------------------------------------------------------

   ImGui::SameLine();
   ImGui::SetNextItemWidth( 50.0 );
   ImGui::Combo( "##Display", &display_format, "DEC\0HEX\0BIN\0\0" );
   ImGui::SeparatorText( "RAM" );
   
   with_StyleVar( ImGuiStyleVar_ChildRounding, 15.0f )
   with_Child("RamChild", ImVec2( ImGui::GetContentRegionAvail().x , ImGui::GetContentRegionAvail().y ), ImGuiChildFlags_Border )
   {
      auto clipper = ImGuiListClipper();
      clipper.Begin( ram_size );

      if ( enable_track )
         clipper.IncludeItemByIndex( track_item );
      
      while( clipper.Step() )
      {
         for ( auto idx = clipper.DisplayStart; idx < clipper.DisplayEnd; ++idx )
         {
            with_ID( idx )
            {
               RAM_Display( static_cast<RAMFormat>( display_format ), idx );

               if ( enable_track && idx == track_item )
               {
                  ImGui::SetScrollHereY( 0 );
                  enable_track = false;
               }
            }
         }
      }
      
   }
}


auto
Hack::Emulator::RAM_Display( RAMFormat fmt, int idx ) -> void
{
   static auto& ram = computer_.RAM();

   auto& data = ram[static_cast<Memory::size_type>( idx )];

   switch ( fmt )
   {
   case RAMFormat::DECIMAL:
   {
      auto value = Hack::Utils::unsigned_to_signed_16( data );
      ImGui::AlignTextToFramePadding();
      ImGui::Text( "%d", idx );
      ImGui::SameLine( 60 );
      ImGui::InputScalar( "##ram", ImGuiDataType_S16, &value );

      if ( value != data )
      {
         data = Hack::Utils::signed_to_unsigned_16( value );
      }
      return;
   }
   
   case RAMFormat::HEX:
   {
      auto value = data;
      ImGui::AlignTextToFramePadding();
      ImGui::Text( "%d", idx );
      ImGui::SameLine( 60 );
      ImGui::InputScalar( "##ram", ImGuiDataType_U16, &value, nullptr, nullptr, "%04X" );

      if ( value != data )
      {
         data = value;
      }

      return;
   }

   case RAMFormat::BINARY:
   {
      auto binary = Hack::Utils::to_binary16_string( data );

      ImGui::AlignTextToFramePadding();
      ImGui::Text( "%d", idx );
      ImGui::SameLine( 60 );
      ImGui::InputText( "rom", &binary );

      if ( auto const value = Hack::Utils::binary_to_uint16( binary ); value )
      {
         if ( *value != data )
         {
            data = *value;
         }
      }
      return;
   }
   
   default:
      std::cerr << "Unknown Format in RAM Display\n";
      break;
   
   }  // switch( fmt )
}


auto
Hack::Emulator::Screen_GUI() -> void
{
   static constexpr auto screen_start  = Computer::screen_start_address;
   static constexpr auto screen_finish = Computer::screen_end_address; 

   static auto enable_track   = false;
   static auto track_item     = std::uint16_t{ screen_start };
   static auto display_format = 0;

   if ( ImGui::Button( "Find" ) )
   {
      enable_track = true;
   } 

   ImGui::SameLine();
   ImGui::SetNextItemWidth( 55.0 );
   ImGui::InputScalar( "##address", ImGuiDataType_U16, &track_item );
   
   if ( track_item >= screen_finish )  
      track_item = screen_finish - 1;
   else if ( track_item < screen_start )
      track_item = screen_start;

   ImGui::SameLine();
   if( ImGui::Button( "Clear" ) )
   {
      ImGui::OpenPopup( "Clear Screen RAM?" );
   }
   
   // "Clear Screen RAM?" -------------------------------------------------------------------------
   with_StyleVar( ImGuiStyleVar_PopupRounding, 10.0 )
   with_StyleVar( ImGuiStyleVar_WindowTitleAlign, ImVec2{ 0.5, 0.5 } )
   {
      auto clear_screen_RAM_popup_open = true;
      with_PopupModal( "Clear Screen RAM?", &clear_screen_RAM_popup_open )
      {
         ImGui::TextUnformatted( "This action cannot be undone" );
         ImGui::Spacing();
         ImGui::Indent( 30.0 );
         if ( ImGui::Button( "Confirm" ) )
         {
            computer_.clear_screen();
            ImGui::CloseCurrentPopup();
         }
      
         ImGui::SameLine( 120.0 ); 
         
         if ( ImGui::Button( "Cancel" ) )
         {
            ImGui::CloseCurrentPopup();
         }
      }
   }

   // ---------------------------------------------------------------------------------------------

   ImGui::SameLine();
   ImGui::SetNextItemWidth( 50.0 );
   ImGui::Combo( "##Display", &display_format, "DEC\0HEX\0BIN\0\0" );
   ImGui::SeparatorText( "Screen RAM" );
   
   with_StyleVar( ImGuiStyleVar_ChildRounding, 15.0f )
   with_Child("ScreenRamChild", ImVec2( ImGui::GetContentRegionAvail().x , ImGui::GetContentRegionAvail().y ), ImGuiChildFlags_Border )
   {
      auto clipper = ImGuiListClipper();
      clipper.Begin( screen_finish - screen_start );

      if ( enable_track )
         clipper.IncludeItemByIndex( static_cast<int>( track_item - screen_start ) );
      
      while( clipper.Step() )
      {
         for ( auto idx = clipper.DisplayStart; idx < clipper.DisplayEnd; ++idx )
         {
            auto const screen_index = idx + static_cast<int>( screen_start );

            with_ID( screen_index )
            {
               RAM_Display( static_cast<RAMFormat>( display_format ), screen_index );

               if ( enable_track && screen_index == track_item )
               {
                  ImGui::SetScrollHereY( 0 );
                  enable_track = false;
               }
            }
         }
      } 
   }
}


auto
Hack::Emulator::displays_errors() -> void
{  
   // only replace the user_error with nullopt once one of the error popups has been closed by
   // the user
   if ( 
      file_error_popup( user_error_ ? user_error_->msg : "" )             ||
      parse_error_popup( user_error_ ? user_error_->msg : "" )            ||
      unsupported_filetype_popup( user_error_ ? user_error_->msg : "" )   ||
      memory_access_error_popup( user_error_ ? user_error_->msg : "" )
     )
   {
      user_error_ = std::nullopt;
   }

   if ( !user_error_ || !user_error_->activate )
   {
      return;
   }


   switch ( user_error_->type )
   {
      case Error_t::PARSE_ERROR:
      {
         ImGui::OpenPopup( "File Parse Error" );
         break;
      }

      case Error_t::FILE_ERROR:
      {
         ImGui::OpenPopup( "File Loading Error" );
         break;
      }

      case Error_t::UNSUPPORTED_FILETYPE:
      {
         ImGui::OpenPopup( "Unsupported File Error" );
         break;
      }

      case Error_t::MEMORY_ACCESS_ERROR:
      {
         ImGui::OpenPopup( "Memory Access Error" );
         break;
      }
   }

   // only called the openpop once
   user_error_->activate = false;

}

// for testing
auto
Hack::Emulator::blacken_screen() -> void
{
   auto begin = computer_.screen_begin();
   auto end   = computer_.screen_end();

   while ( begin != end )
   {
      *begin = 0b1111'1111'1111'1111;
      ++begin;
   }
}



namespace   // ------------------------------------------------------------------------------------
{


auto 
file_error_popup( std::string_view error_msg ) -> bool
{
   auto closed = false;

   with_StyleVar( ImGuiStyleVar_PopupRounding, 10.0 )
   with_StyleVar( ImGuiStyleVar_WindowTitleAlign, ImVec2{ 0.5, 0.5 } )
   {
      auto popup_open = true;
      with_PopupModal( "File Loading Error", &popup_open )
      {
         ImGui::TextUnformatted( "Error - Failed to load file:" );
         ImGui::TextUnformatted( error_msg.data() );

         ImGui::Spacing();
         
         if ( ImGui::Button( "Okay" ) )
         {
            ImGui::CloseCurrentPopup();
            closed = true;
         }
      }
   }
   return closed;
}


auto 
parse_error_popup( std::string_view error_msg  ) -> bool
{
   auto closed = false;

   with_StyleVar( ImGuiStyleVar_PopupRounding, 10.0 )
   with_StyleVar( ImGuiStyleVar_WindowTitleAlign, ImVec2{ 0.5, 0.5 } )
   {
      auto popup_open = true;
      with_PopupModal( "File Parse Error", &popup_open )
      {  
         ImGui::Spacing();
         ImGui::TextUnformatted( error_msg.data() );
         ImGui::Spacing();
         
         if ( ImGui::Button( "Okay" ) )
         {
            ImGui::CloseCurrentPopup();
            closed = true;
         }
      }
   }

   return closed;
}


auto 
unsupported_filetype_popup( std::string_view path ) -> bool
{
   auto closed = false;

   with_StyleVar( ImGuiStyleVar_PopupRounding, 10.0 )
   with_StyleVar( ImGuiStyleVar_WindowTitleAlign, ImVec2{ 0.5, 0.5 } )
   {
      auto popup_open = true;
      with_PopupModal( "Unsupported File Error", &popup_open )
      {
         ImGui::Text( "Could not open file: %s", path.data() );
         ImGui::TextUnformatted( "Unsupported File Format" );
         
         ImGui::Spacing();
         
         if ( ImGui::Button( "Okay" ) )
         {
            ImGui::CloseCurrentPopup();
            closed = true;
         }
      }
   }
   return closed;
}


auto 
memory_access_error_popup( std::string_view msg ) -> bool
{
   auto closed = false;

   with_StyleVar( ImGuiStyleVar_PopupRounding, 10.0 )
   with_StyleVar( ImGuiStyleVar_WindowTitleAlign, ImVec2{ 0.5, 0.5 } )
   {
      auto popup_open = true;
      with_PopupModal( "Memory Access Error", &popup_open )
      { 
         ImGui::TextUnformatted( msg.data() );
         
         ImGui::Spacing();
         
         if ( ImGui::Button( "Okay" ) )
         {
            ImGui::CloseCurrentPopup();
            closed = true;
         }
      }
   }
   return closed;
}


}  // namespace -----------------------------------------------------------------------------------
