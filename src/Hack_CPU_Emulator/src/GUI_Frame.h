/**
 * @file    GUI_Frame.h
 * @author  William Weston (wjtWeston@protonmail.com)
 * @brief   RAII class to encapsulate one frame of GUI animation
 * @version 0.1
 * @date    2024-05-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef HACK_2024_05_14_GUI_FRAME_H
#define HACK_2024_05_14_GUI_FRAME_H

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <SDL.h>

namespace Hack
{

class GUI_Frame final 
{
public:

   GUI_Frame( SDL_Renderer* renderer );
   ~GUI_Frame() noexcept;

   GUI_Frame( GUI_Frame const& )                        = delete;
   auto operator=( GUI_Frame const& )     -> GUI_Frame& = delete;
   GUI_Frame( GUI_Frame&& )      noexcept               = default;
   auto operator=( GUI_Frame&& ) noexcept -> GUI_Frame& = default;


private:
   SDL_Renderer* renderer_{};
};
}


inline Hack::GUI_Frame::GUI_Frame( SDL_Renderer* renderer ) 
   : renderer_{ renderer }
{
   // Start the Dear ImGui frame
   ImGui_ImplSDLRenderer2_NewFrame();
   ImGui_ImplSDL2_NewFrame();
   ImGui::NewFrame();
}

inline Hack::GUI_Frame::~GUI_Frame() noexcept
{
   // Rendering
   ImGui::Render();
   SDL_SetRenderDrawColor( renderer_, 0, 0, 0, SDL_ALPHA_OPAQUE );
   SDL_RenderClear( renderer_ );

   // render GUI
   ImGui_ImplSDLRenderer2_RenderDrawData( ImGui::GetDrawData() );

   // draw backbuffer
   SDL_RenderPresent( renderer_ );
}


#endif         // HACK_2024_05_14_GUI_FRAME_H