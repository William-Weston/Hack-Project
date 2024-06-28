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

#include "Hack/Computer.h"  // for Computer

#include <SDL_pixels.h>     // for SDL_PIXELFORMAT_ARGB8888
#include <SDL_render.h>     // for SDL_CreateTexture, SDL_DestroyTexture
#include <SDL_stdinc.h>     // for Uint32
#include <array>            // for array
#include <bitset>           // for bitset
#include <cstddef>          // for size_t



Hack::Screen_Texture::Screen_Texture( Computer::Screen_const_iterator start,        // NOLINT(bugprone-easily-swappable-parameters)
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
   static constexpr auto pixel_size = static_cast<std::size_t>( width ) * static_cast<std::size_t>( height );
   static constexpr auto word_size  = 16;
   static constexpr auto black      = Uint32{ 0x00000000 };
   static constexpr auto white      = Uint32{ 0xFFFFFFFF };

   auto pixels     = std::array<Uint32, pixel_size>{};
   auto word_count = 0u;

   // each row is 32 16-bit words
   for ( auto begin = start_; begin != finish_ ; ++begin, ++word_count )
   {
      auto const word = *begin;
      auto const bits = std::bitset<word_size>{ word };

      for ( auto idx = 0u; idx < word_size; ++idx )
      {
         pixels.at( word_count * word_size + idx) = ( bits[idx] ) ? black : white;
      }
   }

   SDL_UpdateTexture( texture_, nullptr, pixels.data(), width * sizeof( Uint32 ) );
}


auto 
Hack::Screen_Texture::texture() -> SDL_Texture*
{
   return texture_;
}