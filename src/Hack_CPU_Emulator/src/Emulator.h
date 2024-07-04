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

#include "ALUDisplay.h"         // for ALUDisplay
#include "AnimationHandler.h"   // for AnimationHandler
#include "Definitions.h"        // for UserError, Format
#include "DataDisplay.hpp"
#include "GUI_Core/GUI_Core.h"  // for GUI_Core
#include "InternalsDisplay.h"
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
   using RAMDisplay_t = DataDisplay<Computer::RAM_t>;

   GUI_Core           core_;
   Computer           computer_         {};                    // must be initialized before screen_texture_
   Screen_Texture     screen_texture_; 
   ROMDisplay_t       rom_display_      { computer_.ROM(), 0, Computer::ROM_SIZE };
   RAMDisplay_t       ram_display_      { computer_.RAM(), 0, Computer::RAM_SIZE };
   RAMDisplay_t       screen_display_   { computer_.RAM(), Computer::screen_start_address, Computer::screen_end_address };
   InternalsDisplay   internals_        { computer_ };
   ALUDisplay         alu_display_      { computer_ };
   Assembler const    assembler_        {};
   Keyboard_Handler   keyboard_handler_ {};
   AnimationHandler   animation_handler_{};
   std::string        current_file_     {};
   UserError_t        user_error_       {};
   Format             format_           { Format::SIGNED };
   Format             rom_format_       { Format::ASM };
   float              speed_            { 200'000.0F };        // instructions per second to execute on Hack Computer
   float              animation_speed_  { 5.0f  };
   bool               play_             { false };             // step slowing through the program in the Hack computer ROM
   bool               step_             { false };             // execute the next instruction
   bool               run_              { false };             // run program 
   bool               running_          { true  };             // is the emulator running
   bool               open_new_file_    { false };
   bool               animating_        { false };

   auto handle_events() -> void;
   auto update()        -> void;
   auto render()  const -> void;
   
   constexpr auto is_running() const noexcept -> bool { return running_; }

   

   auto handle_keyboard_events()                 -> void;
   auto open_file( std::string const& path )     -> void;

   auto update_Hack_Computer()                   -> void;
   auto update_GUI_interface()                   -> void;
   
   auto menu_bar()                               -> void;

   auto main_window()                            -> void;
   auto command_GUI()                            -> main_options;
   
   auto display_ROM()                            -> void;
   auto display_RAM( Format fmt )                -> void;
   auto display_screen( Format fmt )             -> void;

   auto internals( Format fmt )                  -> void;
   auto display_cpu()                            -> void;
   auto display_errors()                         -> void;
   
   auto launch_animations()                      -> void;

   auto blacken_screen() -> void;      // testing function
};

}  // namespace Hack

#endif      // HACK_2024_03_15_EMULATOR_H