/**
 * @file    GUI_Core.cpp
 * @author  William Weston
 * @brief   Handles GUI initialization and teardown
 * @version 0.1
 * @date    2024-03-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "GUI_Core.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include "SDL_InitError.h"
#include <SDL.h>

// -------------------------------------------- API -----------------------------------------------

Hack::GUI_Core::GUI_Core( std::string_view title, int x_pos, int y_pos, int width, int height, bool fullscreen )
: width_{ width }, height_{ height }
{
   // attempt to initialize SDL
   init_SDL();

   // init the window
   init_window( title, x_pos, y_pos, width, height, fullscreen );

   // get a SDL_Renderer to the window
   init_renderer();
   
   // init Dear ImGui
   init_imgui();

   // Set Draw Background Colour
   SDL_SetRenderDrawColor( pRenderer_.get(), 0, 0, 0, 255 );
}
   
Hack::GUI_Core::~GUI_Core()
{
   // Cleanup
   ImGui_ImplSDLRenderer2_Shutdown();
   ImGui_ImplSDL2_Shutdown();
   ImGui::DestroyContext();

   SDL_Quit();
}


// ------------------------------------- Implementation -------------------------------------------


auto 
Hack::GUI_Core::init_SDL()      -> void
{
   if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER ) < 0 )  								// SDL init failure
   {
      throw SDL_InitError( "SDL_Init Failure" );
   }
}

auto 
Hack::GUI_Core::init_renderer() -> void
{
   pRenderer_.reset( SDL_CreateRenderer( pWindow_.get(), -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED ) );

   if ( !pRenderer_ )	 								// renderer init failure
   {
      throw SDL_InitError( "SDL_CreateRenderer Failure" );
   }
}

auto 
Hack::GUI_Core::init_imgui()    -> void
{
   // Setup Dear ImGui context
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();

   auto& io = ImGui::GetIO();  //( void )io;
   io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
   io.IniFilename = nullptr;                                   // disable .ini file saving
   // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;        // Enable Gamepad Controls

   // Setup Dear ImGui style
   ImGui::StyleColorsDark();
   //ImGui::StyleColorsLight();

   auto& style = ImGui::GetStyle();

   style.FrameRounding  = 5;
   style.GrabRounding   = style.FrameRounding;
   style.WindowRounding = style.FrameRounding; 
   
   // Setup Platform/Renderer backends
   ImGui_ImplSDL2_InitForSDLRenderer( pWindow_.get(), pRenderer_.get() );
   ImGui_ImplSDLRenderer2_Init( pRenderer_.get() );
}

auto 
Hack::GUI_Core::init_window( std::string_view title, int x_pos, int y_pos, int width, int height, bool fullscreen ) -> void
{
   auto const flags = static_cast<SDL_RendererFlags>( 
      fullscreen  ? SDL_WINDOW_FULLSCREEN | SDL_WINDOW_ALLOW_HIGHDPI
                  : SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI );        //  | SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_INPUT_FOCUS; //  | SDL_WINDOW_MOUSE_GRABBED | SDL_WINDOW_INPUT_GRABBED;


   pWindow_.reset( SDL_CreateWindow( title.data(), x_pos, y_pos, width, height, flags ) );

   if ( !pWindow_ ) 
   {
      throw SDL_InitError( "Failed to initialize SDL window: SDL_CreateWindow returned NULL" );
   }
}
