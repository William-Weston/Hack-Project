/**
 * @file    SDL_InitError.h
 * @author  William Weston
 * @brief   SDL Initialization Exception
 * @version 0.1
 * @date    2024-03-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef HACK_2024_03_15_SDL_INITERROR_H_INCLUDED
#define HACK_2024_03_15_SDL_INITERROR_H_INCLUDED

#include <exception>
#include <string>
#include <SDL.h>		      // SDL_GetError
#include <utility>			// move

class SDL_InitError final : public std::exception
{
public:
   SDL_InitError();
   explicit SDL_InitError( std::string msg );

   virtual const char* what() const noexcept override;

private:
   std::string msg_ = {};
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

#endif // HACK_2024_03_15_SDL_INITERROR_H_INCLUDED