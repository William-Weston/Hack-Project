/**
 * @file    Computer.h
 * @author  William Weston
 * @brief   Hack Computer
 * @version 0.1
 * @date    2024-03-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef HACK_EMULATOR_2024_03_11_COMPUTER_H
#define HACK_EMULATOR_2024_03_11_COMPUTER_H

#include "CPU.h"     // for CPU
#include "Memory.h"  // for Memory

#include <array>     // for array
#include <concepts>  // for same_as
#include <cstdint>   // for uint16_t
#include <iterator>  // for input_iterator, iter_value_t, sentinel_for
#include <span>      // for span

namespace Hack
{

template <typename I>
concept RomIterator = 
    std::input_iterator<I>  &&
    std::sentinel_for<I, I> &&
    std::same_as<std::iter_value_t<I>, std::uint16_t>;


class Computer final
{
public:
   static constexpr auto ROM_SIZE             = 32'758u;                         // 32K
   static constexpr auto RAM_SIZE             = Memory::address_space;           // 16K + 8K + 1 byte
   static constexpr auto screen_start_address = Memory::screen_start_address;
   static constexpr auto screen_end_address   = Memory::screen_end_address;

   using Screen_iterator       = Memory::Screen_iterator;
   using Screen_const_iterator = Memory::Screen_const_iterator;
   using word_t                = std::uint16_t;
   using ROM_t                 = std::array<word_t, ROM_SIZE>;

   auto load_rom( std::span<word_t const> instructions ) -> void;

   template <RomIterator Iter>
   auto load_rom( Iter begin, Iter end ) -> void;

   // execute next instruction
   auto execute() -> void;

   constexpr auto RAM()            const noexcept -> Memory const&;
   constexpr auto RAM()                  noexcept -> Memory&;
   constexpr auto ROM()            const noexcept -> ROM_t  const&;
   constexpr auto ROM()                  noexcept -> ROM_t&;

   constexpr auto A_Register()     const noexcept -> word_t;
   constexpr auto D_Register()     const noexcept -> word_t;
   constexpr auto M_Register()     const noexcept -> word_t;

   constexpr auto A_Register()           noexcept -> word_t&;
   constexpr auto D_Register()           noexcept -> word_t&;
   constexpr auto M_Register()           noexcept -> word_t&;

   constexpr auto screen_begin()         noexcept -> Screen_iterator;
   constexpr auto screen_begin()   const noexcept -> Screen_const_iterator;
   constexpr auto screen_cbegin()  const noexcept -> Screen_const_iterator;
   constexpr auto screen_end()           noexcept -> Screen_iterator;
   constexpr auto screen_end()     const noexcept -> Screen_const_iterator;
   constexpr auto screen_cend()    const noexcept -> Screen_const_iterator;

   constexpr auto keyboard()             noexcept -> word_t&;
   constexpr auto keyboard()       const noexcept -> word_t;
   
   constexpr auto pc()             const noexcept -> word_t;
   constexpr auto pc()                   noexcept -> word_t&;

   constexpr auto reset()                noexcept -> void;     // clears everything but ROM
   constexpr auto clear()                noexcept -> void;     // clears everything
   constexpr auto clear_screen()         noexcept -> void;
   constexpr auto clear_ram()            noexcept -> void;
   constexpr auto clear_rom()            noexcept -> void;
   constexpr auto clear_keyboard()       noexcept -> void;
   constexpr auto clear_pc()             noexcept -> void;
   constexpr auto clear_registers()      noexcept -> void;

   constexpr auto rom_size()       const noexcept -> word_t;
   

private:
   Memory RAM_{};
   ROM_t  ROM_{};
   CPU    cpu_{ RAM_ };
   word_t pc_{ 0 };     // program counter address of next instruction in ROM

};

}  // namespace Hack


template <Hack::RomIterator Iter> auto
Hack::Computer::load_rom( Iter begin, Iter end ) -> void
{
   auto count = 0uz;

   while ( begin != end )
   {
      ROM_[count] = *begin;
      ++count;
      ++begin;
   }
   pc_ = 0;
}


constexpr auto 
Hack::Computer::ROM() const noexcept -> ROM_t const&
{
   return ROM_;
}


constexpr auto 
Hack::Computer::ROM() noexcept -> ROM_t&
{
   return ROM_;
}

constexpr auto 
Hack::Computer::RAM() const noexcept -> Memory const&
{
   return RAM_;
}

constexpr auto 
Hack::Computer::RAM() noexcept -> Memory&
{
   return RAM_;
}

constexpr auto 
Hack::Computer::A_Register() const noexcept -> word_t
{
   return cpu_.A_Register();
}

constexpr auto 
Hack::Computer::D_Register() const noexcept -> word_t
{
   return cpu_.D_Register();
}

constexpr auto 
Hack::Computer::M_Register() const noexcept -> word_t
{
   return cpu_.M_Register();
}

constexpr auto 
Hack::Computer::A_Register()       noexcept -> word_t&
{
   return cpu_.A_Register();
}

constexpr auto 
Hack::Computer::D_Register()       noexcept -> word_t&
{
   return cpu_.D_Register();
}

constexpr auto 
Hack::Computer::M_Register()       noexcept -> word_t&
{
   return cpu_.M_Register();
}


constexpr auto 
Hack::Computer::screen_begin()        noexcept -> Screen_iterator
{
   return RAM_.screen_begin();
}

constexpr auto 
Hack::Computer::screen_begin()   const noexcept -> Screen_const_iterator
{
   return RAM_.screen_begin();
}

constexpr auto 
Hack::Computer::screen_cbegin()  const noexcept -> Screen_const_iterator
{
   return RAM_.screen_cbegin();
}

constexpr auto 
Hack::Computer::screen_end()          noexcept -> Screen_iterator
{
   return RAM_.screen_end();
}

constexpr auto 
Hack::Computer::screen_end()     const noexcept -> Screen_const_iterator
{
   return RAM_.screen_end();
}

constexpr auto 
Hack::Computer::screen_cend()    const noexcept -> Screen_const_iterator
{
   return RAM_.screen_cend();
}


constexpr auto 
Hack::Computer::keyboard()            noexcept -> word_t&
{
   return RAM_.keyboard();
}

constexpr auto 
Hack::Computer::keyboard()       const noexcept -> word_t
{
   return RAM_.keyboard();
}

constexpr auto 
Hack::Computer::pc()             const noexcept -> word_t
{
   return pc_;
}

constexpr auto 
Hack::Computer::pc()                   noexcept -> word_t&
{
   return pc_;
}

constexpr auto 
Hack::Computer::reset()                noexcept -> void
{
   clear_ram();
   clear_pc();
   clear_registers();
   cpu_.reset();
}

constexpr auto 
Hack::Computer::clear()                noexcept -> void
{
   clear_ram();
   clear_rom();
   clear_pc();
   clear_registers();
   cpu_.reset();
}

constexpr auto 
Hack::Computer::clear_screen()         noexcept -> void
{
   RAM_.clear_screen();
}

constexpr auto 
Hack::Computer::clear_ram()            noexcept -> void
{
   RAM_.clear();
}

constexpr auto 
Hack::Computer::clear_rom()            noexcept -> void
{
   ROM_.fill( 0 );
}

constexpr auto 
Hack::Computer::clear_keyboard()       noexcept -> void
{
   RAM_.clear_keyboard();
}

constexpr auto 
Hack::Computer::clear_pc()             noexcept -> void
{
   pc_ = 0;
}

constexpr auto 
Hack::Computer::clear_registers()        noexcept -> void
{
   cpu_.A_Register() = 0;
   cpu_.D_Register() = 0;
}

constexpr auto 
Hack::Computer::rom_size() const noexcept -> word_t
{
   return ROM_SIZE;
}

#endif      // HACK_EMULATOR_2024_03_11_COMPUTER_H