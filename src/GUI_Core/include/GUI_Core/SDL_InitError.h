/**
 * @file    SDL_InitError.h
 * @author  William Weston (wjtWeston@protonmail.com)
 * @brief   SDL Initialization Exception
 * @version 0.1
 * @date    2024-06-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef INCLUDE_2024_06_15_SDL_INITERROR_H
#define INCLUDE_2024_06_15_SDL_INITERROR_H


#include <exception>
#include <string>
#include <SDL.h>		      // SDL_GetError
#include <utility>			// move

class SDL_InitError final : public std::exception
{
public:
   SDL_InitError();
   explicit SDL_InitError( std::string msg );

   virtual auto what() const noexcept -> const char* override;

private:
   std::string msg_{};
};



inline
SDL_InitError::SDL_InitError()
   : exception()
   , msg_( SDL_GetError() )
{}


inline
SDL_InitError::SDL_InitError( std::string msg )
   : exception()
   , msg_( std::move( msg ) )
{
   msg_ = msg_ + ": " + SDL_GetError();
}


inline auto 
SDL_InitError::what() const noexcept -> const char*
{
   return msg_.c_str();
}


#endif   // INCLUDE_2024_06_15_SDL_INITERROR_H