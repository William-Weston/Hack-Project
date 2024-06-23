/**
 * @file    GUI_Core.h
 * @author  William Weston (wjtWeston@protonmail.com)
 * @brief   Handle GUI initialization and teardown
 * @version 0.1
 * @date    2024-06-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef INCLUDE_2024_06_15_GUI_CORE_H
#define INCLUDE_2024_06_15_GUI_CORE_H


#include <SDL_render.h>       // for SDL_Renderer, SDL_DestroyRenderer
#include <SDL_video.h>        // for SDL_Window, SDL_DestroyWindow

#include <memory>             // for unique_ptr
#include <string_view>        // for string_view

namespace Hack
{

class GUI_Core final
{
public:

   [[nodiscard]] explicit GUI_Core( std::string_view title, int width, int height, bool fullscreen );
   ~GUI_Core();

   constexpr auto window()   const noexcept -> SDL_Window*;
   constexpr auto renderer() const noexcept -> SDL_Renderer*;

   constexpr auto width()    const noexcept -> int;
   constexpr auto height()   const noexcept -> int;

private:
   using window_del_t   = void ( * ) ( SDL_Window* );
   using renderer_del_t = void ( * ) ( SDL_Renderer* );
   using pWindow_t      = std::unique_ptr<SDL_Window,   window_del_t>;
   using pRenderer_t    = std::unique_ptr<SDL_Renderer, renderer_del_t>;


   pWindow_t   pWindow_    = { nullptr, SDL_DestroyWindow };
   pRenderer_t pRenderer_  = { nullptr, SDL_DestroyRenderer };
   int         width_      = 0;
   int         height_     = 0;

   auto init_SDL()      -> void;
   auto init_renderer() -> void;
   auto init_imgui()    -> void;
   auto init_window( std::string_view title, int width, int height, bool fullscreen ) -> void;
   
};

}  // namespace Hack

// ------------------------------------------ API: constexpr --------------------------------------

constexpr auto 
Hack::GUI_Core::window()   const noexcept -> SDL_Window*
{
   return pWindow_.get();
}

constexpr auto 
Hack::GUI_Core::renderer() const noexcept -> SDL_Renderer*
{
   return pRenderer_.get();
}

constexpr auto 
Hack::GUI_Core::width()   const noexcept -> int
{
   return width_;
}

constexpr auto 
Hack::GUI_Core::height()  const noexcept -> int
{
   return height_;
}



#endif   // INCLUDE_2024_06_15_GUI_CORE_H