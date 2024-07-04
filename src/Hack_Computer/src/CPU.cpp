/**
 * @file    CPU.cpp
 * @author  William Weston
 * @brief   Emulated Hack CPU
 * @version 0.1
 * @date    2024-03-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "CPU.h"

#include "ALU.h"
#include "Hack/Utilities/utilities.hpp"      // is_a_instruction

#include <bitset>


/**
 * @brief   Execute the instruction
 * 
 * @param instruction   the instruction to execute
 * @return word_t       the next instruction to fetch from the instruction ROM
 */
auto 
Hack::CPU::execute_instruction( word_t instruction ) -> word_t
{
   if ( Hack::Utils::is_a_instruction( instruction ) )
   {
      return do_a_instruction( instruction );
   }
   
   return do_c_instruction( instruction );
}



// ----------------------------------------- Implementation ---------------------------------------

/**
 * @brief   Perform A-instruction
 * 
 * @param instruction   the instruction to execute
 * @return word_t       the next instruction to load from ROM
 */
auto 
Hack::CPU::do_a_instruction( word_t instruction ) -> word_t
{
   A_Register_ = instruction;

   return ++PC_;
}


/**
 * @brief   Execute C-instruction
 * 
 * @param instruction   the instruction to execute
 * @return word_t       next instruction to be fetched from ROM
 * 
 *    C-Instruction: 111 a cccccc ddd jjj
 */
auto 
Hack::CPU::do_c_instruction( word_t instruction ) -> word_t
{
   // 1111'1100'0000'0000
   // 5432'1098'7654'3210
   // 111a'cccc'ccdd'djjj
   auto const bits = std::bitset<16>( instruction );

   // if the 'a' bit is set then the y input to the ALU comes from the M register (RAM_[A]) else from the A register
   auto const a_bit = bits.test( 12 );
   auto const x     = D_Register_;
   auto const y     = [&]( bool const bit ) { return ( bit ) ? RAM_[A_Register_] : A_Register_; }( a_bit );
   auto const zx    = bits.test( 11 );
   auto const nx    = bits.test( 10 );
   auto const zy    = bits.test( 9 );
   auto const ny    = bits.test( 8 );
   auto const f     = bits.test( 7 );
   auto const no    = bits.test( 6 );
   
   // ALU: x = D Register, y = A or M Register
   //    in          x, y, zx, nx, zy, ny, f, no
   auto const [comp, zr, ng] = ALU( ALU_in{ x, y, zx, nx, zy, ny, f, no } );
   
   auto const store_A = bits.test( 5 );
   auto const store_D = bits.test( 4 );
   auto const store_M = bits.test( 3 );
   auto const address = A_Register_;      // save current A_Register value to access M Regisiter

   ALU_output_ = comp;   // store the output of the ALU

   if ( store_A ) { A_Register_   = comp; }
   if ( store_D ) { D_Register_   = comp; }
   if ( store_M ) { RAM_[address] = comp; }

   auto const jmp_lt = bits.test( 2 );
   auto const jmp_eq = bits.test( 1 );
   auto const jmp_gt = bits.test( 0 );

   // jump to instruction number in A Register if comp < 0
   if ( jmp_lt )
   {
      if ( ng )      // ng bit from ALU indicates a negative result
      {
         PC_ = A_Register_;
         return PC_;
      }
   }

   // jump to instruction number in A Register if comp == 0
   if ( jmp_eq )
   {
      if ( zr )   // zr bit from ALU indicates result was zero
      {
         PC_ = A_Register_;
         return PC_;
      }
   }

   // jump to instruction number in A Register if comp > 0
   if ( jmp_gt )
   {
      if ( !( ng || zr ) )
      {
         PC_ = A_Register_;
         return PC_;
      }
   }

   return ++PC_;     // increment PC_ and return
}


// evaluate an instruction on the ALU given two inputs
auto 
Hack::CPU::evaluate( word_t x, word_t y, word_t instruction )  -> word_t
{
   // 1111'1100'0000'0000
   // 5432'1098'7654'3210
   // 111a'cccc'ccdd'djjj
   auto const bits = std::bitset<16>( instruction );
 
   auto const zx    = bits.test( 11 );
   auto const nx    = bits.test( 10 );
   auto const zy    = bits.test( 9 );
   auto const ny    = bits.test( 8 );
   auto const f     = bits.test( 7 );
   auto const no    = bits.test( 6 );
   
   // ALU: x = D Register, y = A or M Register
   //    in          x, y, zx, nx, zy, ny, f, no
   return ALU( ALU_in{ x, y, zx, nx, zy, ny, f, no } ).out;
}