/**
 * @file    ALU.h
 * @author  William Weston
 * @brief   Hack ALU
 * @version 0.1
 * @date    2024-03-11
 * 
 * @copyright Copyright (c) 2024
 * 
 * 
 *    Chip Name:  ALU
 * 
 *       Input:
 *                   x[16]          16-bit data input
 *                   y[16]          16-bit data input
 *                   zx             zero the x input
 *                   nx             negate the x input
 *                   zy             zero the y input
 *                   ny             negate the y input
 *                   f              if ( f == 1 )  out = add( x, y ),   else out = and( x, y )
 *                   no             negate the out output
 * 
 *       Output:        
 *                   out[16]        16-bit output
 *                   zr             if ( out == 0 ):  zr = 1
 *                                  else:             zr = 0
 *                   ng             if ( out < 0 ):   ng = 1  
 *                                  else:             ng = 0
 * 
 *       Function:
 *                   if zx          x = 0          •  16-bit zero constant
 *                   if nx          x = !x         •  Bit-wise negation
 *                   if zy          y = 0          •  16-bit zero constant
 *                   if ny          y = !y         •  Bit-wise negation
 *                   if f           out = x + y    •  Integer two's complement addition
 *                   else           out = x & y    •  Bit-wise AND
 *                   if no          out = !out     •  Bit-wise negation
 *                   if out == 0    zr = 1         •  16-bit equality comparison
 *                   else           zr = 0
 *                   if out < 0     ng = 1         •  two's complement comparison
 *                   else           ng = 0
 * 
 *       Comment:    The overflow bit is ignored
 * 
 *    out( x, y, zx, nx, zy, ny, f, no )  =  0, 1, -1,
 *                                           x, y, ~x, ~y, -x, -y,
 *                                           x + 1, y + 1, x - 1, y - 1,
 *                                           x + y, x - y, y - x,
 *                                           x & y, x | y
 */
#ifndef HACK_EMULATOR_2024_03_11_ALU_H
#define HACK_EMULATOR_2024_03_11_ALU_H

#include <array>
#include <cstdint>

namespace Hack
{

struct ALU_in;
struct ALU_out;

/**
 * @brief   Emulates the Hack ALU
 * 
 * @param   in          x, y, zx, nx, zy, ny, f, no
 * @return  ALU_out     out( x, y, zx, nx, zy, ny, f, no )  =  0, 1, -1,
 *                                                             x, y, ~x, ~y, -x, -y,
 *                                                             x + 1, y + 1, x - 1, y - 1,
 *                                                             x + y, x - y, y - x,
 *                                                             x & y, x | y
 * 
 *    Function:
 *                if zx          x = 0          •  16-bit zero constant
 *                if nx          x = !x         •  Bit-wise negation
 *                if zy          y = 0          •  16-bit zero constant
 *                if ny          y = !y         •  Bit-wise negation
 *                if f           out = x + y    •  Integer two's complement addition
 *                else           out = x & y    •  Bit-wise AND
 *                if no          out = !out     •  Bit-wise negation
 *                if out == 0    zr = 1         •  16-bit equality comparison
 *                else           zr = 0
 *                if out < 0     ng = 1         •  two's complement comparison
 *                else           ng = 0   
 */
constexpr auto ALU( ALU_in in ) -> ALU_out;


}  // namespace Hack


// ---------------------------------------- Implementation ----------------------------------------

struct Hack::ALU_in
{
   std::uint16_t  x;
   std::uint16_t  y;
   bool           zx;
   bool           nx;
   bool           zy;
   bool           ny;
   bool           f;
   bool           no;
};

struct Hack::ALU_out
{
   std::uint16_t  out;
   bool           zr;
   bool           ng;
};

constexpr auto 
Hack::ALU( ALU_in in ) -> ALU_out
{
   constexpr std::uint16_t sign_bit = 0b1000'0000'0000'0000;

   if ( in.zx )
   {
      in.x = 0;
   }

   if ( in.nx )
   {
      in.x = ~in.x;
   }

   if ( in.zy )
   {
      in.y = 0;
   }

   if ( in.ny )
   {
      in.y = ~in.y;
   }

   std::uint16_t out = ( in.f )  ? in.x + in.y   
                                 : in.x & in.y;

   if ( in.no )
   {
      out = ~out;
   }

   bool const zr = ( out == 0 ) ? true : false;

   bool const ng = ( out & sign_bit ) ? true : false;      // if out < 0: ng = 1, else: ng = 0

   return { out, zr, ng };
}

#endif      // HACK_EMULATOR_2024_03_11_ALU_H