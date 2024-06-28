/**
 * @file    GUI_Frame.cpp
 * @author  William Weston (wjtWeston@protonmail.com)
 * @brief   RAII class to encapsulate one frame of GUI animation
 * @version 0.1
 * @date    2024-06-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "GUI_Frame.h"

#include <imgui.h>                    // for GetDrawData, NewFrame, Render
#include <imgui_impl_sdl2.h>          // for ImGui_ImplSDL2_NewFrame
#include <imgui_impl_sdlrenderer2.h>  // for ImGui_ImplSDLRenderer2_NewFrame
#include <SDL_pixels.h>               // for SDL_ALPHA_OPAQUE
#include <SDL_render.h>               // for SDL_RenderClear, SDL_RenderPresent

[[nodiscard]]
Hack::GUI_Frame::GUI_Frame( SDL_Renderer* renderer ) 
   : renderer_{ renderer }
{
   // Start the Dear ImGui frame
   ImGui_ImplSDLRenderer2_NewFrame();
   ImGui_ImplSDL2_NewFrame();
   ImGui::NewFrame();
}

Hack::GUI_Frame::~GUI_Frame() noexcept
{
   // Rendering
   ImGui::Render();
   SDL_SetRenderDrawColor( renderer_, 0, 0, 0, SDL_ALPHA_OPAQUE );
   SDL_RenderClear( renderer_ );

   // render GUI
   ImGui_ImplSDLRenderer2_RenderDrawData( ImGui::GetDrawData(), renderer_ );

   // draw backbuffer
   SDL_RenderPresent( renderer_ );
}