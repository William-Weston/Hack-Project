/**
 * @file Screen_Texture.cpp
 * @author William Weston (wjtWeston@protonmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "Screen_Texture.h"

#include <Hack/Computer.h>
#include <SDL.h>

#include <array>
#include <bitset>


Hack::Screen_Texture::Screen_Texture( Computer::Screen_const_iterator start, 
                                      Computer::Screen_const_iterator finish, 
                                      SDL_Renderer* renderer )
   :  start_{ start },
      finish_{ finish },
      texture_{ SDL_CreateTexture( renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, width, height ) }
                                                                                
{
}                  
   
Hack::Screen_Texture::~Screen_Texture()
{
   SDL_DestroyTexture( texture_ );
}

auto 
Hack::Screen_Texture::update()  -> void
{
   auto pixels     = std::array<Uint32, width * height>{};
   auto word_count = 0u;

   // each row is 32 16-bit words
   for ( auto begin = start_; begin != finish_ ; ++begin, ++word_count )
   {
      auto const word = *begin;
      auto const bits = std::bitset<16>{ word };

      for ( auto idx = 0u; idx < 16; ++idx )
      {
         pixels[word_count * 16 + idx] = ( bits[idx] ) ? 0x00000000 : 0xFFFFFFFF;
      }
   }

   SDL_UpdateTexture( texture_, nullptr, pixels.data(), width * sizeof( Uint32 ) );
}


auto 
Hack::Screen_Texture::texture() -> SDL_Texture*
{
   return texture_;
}