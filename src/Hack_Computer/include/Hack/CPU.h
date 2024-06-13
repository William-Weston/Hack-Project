/**
 * @file    CPU.h
 * @author  William Weston
 * @brief   Hack CPU
 * @version 0.1
 * @date    2024-03-11
 * 
 * @copyright Copyright (c) 2024
 * 
 *    Chip Name:  CPU
 * 
 *       Input:    
 *          instruction[16]   instruction to execute
 *          inM[16]           the instruction's M input (contents of RAM_[A])  
 *          reset             Signals whether to restart the program (reset == 1)
 *                            or continue executing the program (reset == 0)
 * 
 *       Output:  
 *          outM[16]          Written to RAM_[addressM], the instruction's M output
 *          addressM[15]      At which address to write?
 *          writeM[1]         Write to memory?
 *          pc[15]            Address of next instruction
 * 
 *    
 *    Instruction Set:
 *       
 *       A-instruction:    Sets the A register to some 15-bit value.
 *       C-instruction     Answers three questions:   
 *                            •  what to compute (an ALU operation, denoted comp)
 *                            •  where to store the computed value (dest)
 *                            •  what to do next (jump)
 *       
 *       A-Instruction:
 *          
 *          Symbolic:   @xxx                 •  xxx is a decimal value ranging from 0 to 32'767,  
 *                                              or a symbol bound to such a decimal value
 *          Binary:     0vvvvvvvvvvvvvvv     •  vv ... v = 15-bit value of xxx
 *    
 *       C-Instruction:
 *       
 *          Symbolic:   dest = comp; jump    •  comp is mandatory
 *                                              if dest is empty, the '=' is omitted
 *                                              if jump is empty, the ';' is omitted
 *          Binary:     111accccccdddjjj
 */
#ifndef HACK_EMULATOR_2024_03_11_CPU_H
#define HACK_EMULATOR_2024_03_11_CPU_H


#include "Memory.h"

#include <cstdint>

namespace Hack
{

class CPU final
{
public:
   using word_t = std::uint16_t;

   constexpr explicit CPU( Memory& memory ) noexcept;
   constexpr ~CPU() noexcept = default;

   CPU( CPU const& )                    = delete;
   CPU( CPU&& )                         = delete;
   auto operator=( CPU const& ) -> CPU& = delete;
   auto operator=( CPU&& )      -> CPU& = delete;

   // returns address of next instruction to execute
   auto execute_instruction( word_t instruction ) -> word_t;

   constexpr auto A_Register() const noexcept -> word_t;
   constexpr auto D_Register() const noexcept -> word_t;
   constexpr auto M_Register() const          -> word_t;

   constexpr auto A_Register()       noexcept -> word_t&;
   constexpr auto D_Register()       noexcept -> word_t&;
   constexpr auto M_Register()                -> word_t&;

   constexpr auto set_A_Register( word_t value ) noexcept -> void;
   constexpr auto set_D_Register( word_t value ) noexcept -> void;

   constexpr auto reset()                        noexcept -> void;

private:
   word_t  A_Register_ = 0;
   word_t  D_Register_ = 0;
   word_t  PC_         = 0;      
   Memory& RAM_;

   auto do_a_instruction( word_t instruction ) -> word_t;
   auto do_c_instruction( word_t instruction ) -> word_t;
};

}     // namespace Hack


// ---------------------------------------- Implementation ----------------------------------------


constexpr 
Hack::CPU::CPU( Hack::Memory& memory ) noexcept
   :  RAM_{ memory }
{

}

constexpr auto
Hack::CPU::A_Register() const noexcept -> word_t
{
   return A_Register_;
}

constexpr auto
Hack::CPU::D_Register() const noexcept -> word_t
{
   return D_Register_;
}

constexpr auto
Hack::CPU::M_Register() const -> word_t
{
   return RAM_[A_Register_];
}

constexpr auto
Hack::CPU::A_Register()       noexcept -> word_t&
{
   return A_Register_;
}

constexpr auto
Hack::CPU::D_Register()       noexcept -> word_t&
{
   return D_Register_;
}

constexpr auto
Hack::CPU::M_Register() -> word_t&
{
   return RAM_[A_Register_];
}


constexpr auto
Hack::CPU::set_A_Register( word_t value ) noexcept -> void
{
   A_Register_ = value;
}

constexpr auto
Hack::CPU::set_D_Register( word_t value ) noexcept -> void
{
   D_Register_ = value;
}

constexpr auto
Hack::CPU::reset()                        noexcept -> void
{
   A_Register_ = 0;
   D_Register_ = 0;
   PC_         = 0;
}

#endif      // HACK_EMULATOR_2024_03_11_CPU_H