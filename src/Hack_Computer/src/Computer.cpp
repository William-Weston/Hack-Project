/**
 * @file    Computer.cpp
 * @author  William Weston
 * @brief   Hack Computer
 * @version 0.1
 * @date    2024-03-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Computer.h"

#include <algorithm>    // for __copy_fn, copy
#include <stdexcept>    // for runtime_error
#include <string>       // for operator+, to_string

auto 
Hack::Computer::load_rom( std::span<word_t const> instructions ) -> void
{   
   if ( instructions.size() > ROM_SIZE )
   {
      throw std::runtime_error( "ROM overflow: " + std::to_string( instructions.size() ) );        // TODO: display proper error message 
   }

   rng::copy( instructions, ROM_.begin() );

   pc_ = 0;
}

auto 
Hack::Computer::execute() -> void
{  
   pc_ = cpu_.execute_instruction( ROM_.at( pc_ ) );
}