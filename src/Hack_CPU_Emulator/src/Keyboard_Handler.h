/**
 * @file    Keyboard_Handler.h
 * @author  William Weston
 * @brief   Handle Keyboard Input
 * @version 0.1
 * @date    2024-03-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef HACK_2024_03_19_KEYBOARD_HANDLER_H
#define HACK_2024_03_19_KEYBOARD_HANDLER_H

#include <SDL_scancode.h>        // for SDL_Scancode, SDL_SCANCODE_LSHIFT, SDL_SCA...
#include <SDL_stdinc.h>          // for Uint8

namespace Hack
{

class Keyboard_Handler final
{
public:
  
   constexpr Keyboard_Handler()  noexcept = default;
   constexpr ~Keyboard_Handler() noexcept = default;
   constexpr Keyboard_Handler( Keyboard_Handler const& )                            = default;
   constexpr Keyboard_Handler( Keyboard_Handler&& )   noexcept                      = default;
   constexpr auto operator=( Keyboard_Handler const& )         -> Keyboard_Handler& = default;
   constexpr auto operator=( Keyboard_Handler&& )     noexcept -> Keyboard_Handler& = default;

   constexpr auto isKeyDown( SDL_Scancode key ) const noexcept -> bool;
   constexpr auto isKeyUp( SDL_Scancode key )   const noexcept -> bool;
   constexpr auto isShiftDown()                 const noexcept -> bool;
   constexpr auto isShiftUp()                   const noexcept -> bool;

   // handle keyboard events
   auto onKeyDown() noexcept -> void;
   auto onKeyUp()   noexcept -> void;

private:
   Uint8 const* keystates_ = nullptr;
};

}     // namespace Hack


constexpr auto 
Hack::Keyboard_Handler::isKeyDown( SDL_Scancode key ) const noexcept -> bool
{
   if ( keystates_ )
   {
      if ( keystates_[ key ] == 1 )   // key was pressed
      {
         return true;
      }
      else
      {
         return false;
      }
   }

   return false;
}

constexpr auto 
Hack::Keyboard_Handler::isKeyUp( SDL_Scancode key )   const noexcept -> bool
{
   return !isKeyDown( key );
}

constexpr auto 
Hack::Keyboard_Handler::isShiftDown() const noexcept -> bool
{
   return isKeyDown( SDL_SCANCODE_LSHIFT ) || isKeyDown( SDL_SCANCODE_RSHIFT );
}

constexpr auto 
Hack::Keyboard_Handler::isShiftUp() const noexcept -> bool
{
   return !isShiftDown();
}

#endif      // HACK_2024_03_19_KEYBOARD_HANDLER_H