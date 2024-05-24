/**
 * @file    Keyboard_Handler.cpp
 * @author  William Weston
 * @brief   Handle Keyboard Input
 * @version 0.1
 * @date    2024-03-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Keyboard_Handler.h"

#include <SDL.h>


auto 
Hack::Keyboard_Handler::onKeyDown() noexcept -> void
{
   keystates_ = SDL_GetKeyboardState( nullptr );
}

auto 
Hack::Keyboard_Handler::onKeyUp()   noexcept -> void
{
   keystates_ = SDL_GetKeyboardState( nullptr );
}