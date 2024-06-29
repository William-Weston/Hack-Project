/**
 * @file    Emulator.h
 * @author  William Weston
 * @brief   Hack Emulator GUI Application
 * @version 0.1
 * @date    2024-03-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef HACK_2024_03_15_EMULATOR_H
#define HACK_2024_03_15_EMULATOR_H


#include "Definitions.h"        // for UserError, Format
#include "DataDisplay.hpp"
#include "GUI_Core/GUI_Core.h"  // for GUI_Core
#include "Keyboard_Handler.h"   // for Keyboard_Handler
#include "Screen_Texture.h"     // for Screen_Texture

#include <Hack/Assembler.h>     // for Assembler
#include <Hack/Computer.h>      // for Computer
#include <Hack/Disassembler.h>  // for Disassembler

#include <SDL_render.h>         // for SDL_Renderer
#include <SDL_video.h>          // for SDL_Window
#include <optional>             // for optional
#include <string>               // for string
#include <string_view>          // for string_view


namespace Hack
{

class Emulator final
{
public:
   explicit Emulator( std::string_view title, int width, int height, bool fullscreen );
   ~Emulator() noexcept = default;

   Emulator( Emulator const& )                    = delete;
   Emulator( Emulator&& )                         = delete;
   auto operator=( Emulator const& ) -> Emulator& = delete;
   auto operator=( Emulator&& )      -> Emulator& = delete;

   auto run() -> void;

private:
   using UserError_t  = std::optional<UserError>;
   using ROMDisplay_t = DataDisplay<Computer::ROM_t>;

   GUI_Core           core_;
   Computer           computer_{};              // must be initialized before screen_
   Screen_Texture     screen_texture_;
   ROMDisplay_t       rom_display{ computer_.ROM(), 0, Computer::ROM_SIZE };
   Assembler const    assembler_{};
   Keyboard_Handler   keyboard_handler_{};
   std::string        current_file_{};
   UserError_t        user_error_{};
   float              speed_{ 0.33F };            // instructions per second to execute on Hack Computer
   bool               play_{ false };             // run the program in the Hack computer ROM
   bool               step_{ false };             // execute the next instruction
   bool               running_{ true };           // is the emulator running
   bool               open_new_file_{ false };
   bool               animating_{ false };

   auto handle_events() -> void;
   auto update()        -> void;
   auto render()  const -> void;
   
   constexpr auto is_running() const noexcept -> bool { return running_; }

   

   auto handle_keyboard_events()                 -> void;
   auto open_file( std::string const& path )     -> void;

   auto update_Hack_Computer()                   -> void;
   auto update_GUI_interface()                   -> void;
   
   auto main_window()                            -> void;
   auto command_GUI()                            -> main_options;
   auto menu_bar()                               -> void;
   
   auto internals()                              -> void;
   auto display_cpu()                            -> void;
   auto display_errors()                         -> void;
   
   auto blacken_screen() -> void;      // testing function
};

}  // namespace Hack

#endif      // HACK_2024_03_15_EMULATOR_H