/**
 * @file    GUI_Frame.h
 * @author  William Weston (wjtWeston@protonmail.com)
 * @brief   RAII class to encapsulate one frame of GUI animation
 * @version 0.1
 * @date    2024-06-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef INCLUDE_2024_06_15_GUI_FRAME_H
#define INCLUDE_2024_06_15_GUI_FRAME_H


class SDL_Renderer;           // forward declaration

namespace Hack
{

class GUI_Frame final 
{
public:
   
   [[nodiscard]] GUI_Frame( SDL_Renderer* renderer );
   ~GUI_Frame() noexcept;

   GUI_Frame( GUI_Frame const& )                        = delete;
   GUI_Frame( GUI_Frame&& )      noexcept               = delete;
   auto operator=( GUI_Frame const& )     -> GUI_Frame& = delete;
   auto operator=( GUI_Frame&& ) noexcept -> GUI_Frame& = delete;

private:
   SDL_Renderer* renderer_;
};

}  // namespace Hack


#endif   // INCLUDE_2024_06_15_GUI_FRAME_H