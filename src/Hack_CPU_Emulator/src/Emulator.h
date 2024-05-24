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

#include "Definitions.h"            // UserError
#include "GUI_Core.h"
#include "Keyboard_Handler.h"
#include "Screen_Texture.h"

#include <Hack/Assembler.h>
#include <Hack/Computer.h>
#include <Hack/Disassembler.h>

#include <SDL.h>

#include <optional> 
#include <string>
#include <string_view>



namespace Hack
{

class Emulator final
{
public:
   explicit Emulator( std::string_view title, int x_pos, int y_pos, int width, int height, bool fullscreen );
   ~Emulator() noexcept = default;

   Emulator( Emulator const& )                    = delete;
   Emulator( Emulator&& )                         = delete;
   auto operator=( Emulator const& ) -> Emulator& = delete;
   auto operator=( Emulator&& )      -> Emulator& = delete;

   auto run() -> void;

private:
   using UserError_t = std::optional<UserError>;

   GUI_Core         core_;
   Computer         computer_{};             // must be initialized before screen_
   Screen_Texture   screen_texture_;
   Assembler        assembler_{};
   Disassembler     disasmblr_{};
   SDL_Window*      window_{};
   SDL_Renderer*    renderer_{};
   Keyboard_Handler keyboard_handler_{};
   std::string      current_file_{};
   UserError_t      user_error_{};
   float            speed_{ 0.33F };            // instructions per second to execute on Hack Computer
   bool             play_{ false };             // run the program in the Hack computer ROM
   bool             step_{ false };             // execute the next instruction
   bool             running_{ true };          // is the emulator running
   bool             open_new_file_{ false };

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
   auto ROM_GUI( bool highlight_pc )             -> void;
   auto ROM_Display( ROMFormat fmt, int idx )    -> void;
   auto RAM_GUI( )                               -> void;
   auto RAM_Display( RAMFormat fmt, int idx )    -> void;
   auto Screen_GUI( )                            -> void;

   auto displays_errors()                        -> void;
   
   auto blacken_screen() -> void;      // testing function
};

}  // namespace Hack

#endif      // HACK_2024_03_15_EMULATOR_H