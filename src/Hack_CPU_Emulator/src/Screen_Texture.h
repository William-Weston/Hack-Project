/**
 * 
*/

#ifndef HACK_2024_05_8_SCREEN_TEXTURE_H
#define HACK_2024_05_8_SCREEN_TEXTURE_H

#include <Hack/Computer.h>       // for Computer

class SDL_Renderer;              
class SDL_Texture;              

namespace Hack
{
class Screen_Texture final
{
public:

   static constexpr auto width  = 512;
   static constexpr auto height = 256;

   Screen_Texture( Computer::Screen_const_iterator start, 
                   Computer::Screen_const_iterator finish, 
                   SDL_Renderer* renderer ); 
   
   ~Screen_Texture();

   Screen_Texture( Screen_Texture const& )                             = delete;
   Screen_Texture( Screen_Texture&& )       noexcept                   = delete;
   auto operator=( Screen_Texture const& )          -> Screen_Texture& = delete;
   auto operator=( Screen_Texture&& )      noexcept -> Screen_Texture& = delete;

   auto update()  -> void;
   auto texture() -> SDL_Texture*;

private:
   using iterator = Computer::Screen_const_iterator;

   iterator const start_; 
   iterator const finish_;

   SDL_Texture*   texture_{}; 
};

}  // namespace Hack
#endif         // HACK_2024_05_8_SCREEN_TEXTURE_H