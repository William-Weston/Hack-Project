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
   clear();
   namespace rng = std::ranges;
   
   if ( instructions.size() > ROM_SIZE )
   {
      throw std::runtime_error( "ROM overflow: " + std::to_string( instructions.size() ) );        // TODO: display proper error message 
   }

   rng::copy( instructions, ROM_.begin() );

}

auto 
Hack::Computer::execute() -> void
{  
   cpu_.execute_instruction( ROM_.at( cpu_.PC() ) );
}


// evaluate an instruction on the ALU based on two inputs, does not effect current state of computer, x would be d register, y would be a/m register
auto 
Hack::Computer::evaluate( word_t x, word_t y, word_t instruction ) -> word_t
{
   return CPU::evaluate( x, y, instruction );
}