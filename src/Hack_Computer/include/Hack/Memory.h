/**
 * @file    Memory.h
 * @author  William Weston
 * @brief   Memory Module for Hack Computer
 * @version 0.1
 * @date 2024-03-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef HACK_EMULATOR_2024_03_11_MEMORY_H
#define HACK_EMULATOR_2024_03_11_MEMORY_H


#include <array>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>      // as_const

namespace Hack
{

class Memory final
{
public:
   static constexpr auto address_space        = 24'577u;    // number of bytes in RAM 16K RAM + 8K memory mapped io + 1 byte keyboard map
   static constexpr auto screen_start_address = 16'384u;
   static constexpr auto screen_end_address   = 24'576u;     // one past the last valid address of the screen 
   static constexpr auto keyboard_address     = 24'576u;

   using value_type            = std::uint16_t;
   using size_type             = std::size_t;
   using difference_type       = std::ptrdiff_t;
   using reference             = value_type&;
   using const_reference       = value_type const&;
   using pointer               = value_type*;
   using const_pointer         = value_type const*;
   using RAM_iterator          = std::array<std::uint16_t, 16384>::iterator;
   using RAM_const_iterator    = std::array<std::uint16_t, 16384>::const_iterator;
   using Screen_iterator       = std::array<std::uint16_t, 81924>::iterator;
   using Screen_const_iterator = std::array<std::uint16_t, 81924>::const_iterator;

   constexpr explicit Memory() noexcept = default;

   auto operator[]( size_type index )       -> reference;
   auto operator[]( size_type index ) const -> const_reference;

   constexpr auto ram_begin()            noexcept -> RAM_iterator;
   constexpr auto ram_begin()      const noexcept -> RAM_const_iterator;
   constexpr auto ram_cbegin()     const noexcept -> RAM_const_iterator;
   constexpr auto ram_end()              noexcept -> RAM_iterator;
   constexpr auto ram_end()        const noexcept -> RAM_const_iterator;
   constexpr auto ram_cend()       const noexcept -> RAM_const_iterator;

   constexpr auto screen_begin()         noexcept -> Screen_iterator;
   constexpr auto screen_begin()   const noexcept -> Screen_const_iterator;
   constexpr auto screen_cbegin()  const noexcept -> Screen_const_iterator;
   constexpr auto screen_end()           noexcept -> Screen_iterator;
   constexpr auto screen_end()     const noexcept -> Screen_const_iterator;
   constexpr auto screen_cend()    const noexcept -> Screen_const_iterator;

   constexpr auto keyboard()             noexcept -> reference;
   constexpr auto keyboard()       const noexcept -> const_reference;

   constexpr auto clear()                noexcept -> void;
   constexpr auto clear_screen()         noexcept -> void;
   constexpr auto clear_ram()            noexcept -> void;
   constexpr auto clear_keyboard()       noexcept -> void;

private:
   std::array<std::uint16_t, 16'384> RAM16K;
   std::array<std::uint16_t, 8'192>  Screen;
   std::uint16_t                     Keyboard;
};

}  // namespace Hack

// ---------------------------------------- Implementation ----------------------------------------

inline auto 
Hack::Memory::operator[] ( size_type index )       -> reference
{
   return const_cast<reference>( std::as_const( *this ).operator[]( index ) );
}

inline auto 
Hack::Memory::operator[] ( size_type index ) const -> const_reference
{
   if ( index < 16'384 )
   {
      return RAM16K[index];
   }
   else if ( index < 24'576 )
   {
      return Screen[index - 16'384];
   }
   else if ( index == 24'576 )
   {
      return Keyboard;
   }
   else
   {
      throw std::out_of_range( "RAM: Memory access out of bounds: " + std::to_string( index ) );
   }
}

constexpr auto 
Hack::Memory::screen_begin() noexcept -> Screen_iterator
{
   return Screen.begin();
}


constexpr auto 
Hack::Memory::ram_begin() noexcept -> RAM_iterator
{
   return RAM16K.begin();
}

constexpr auto 
Hack::Memory::ram_begin() const noexcept -> RAM_const_iterator
{
   return RAM16K.begin();
}

constexpr auto 
Hack::Memory::ram_cbegin() const noexcept -> RAM_const_iterator
{
   return RAM16K.begin();
}

constexpr auto 
Hack::Memory::ram_end()         noexcept  -> RAM_iterator
{
   return RAM16K.end();
}

constexpr auto 
Hack::Memory::ram_end()        const noexcept -> RAM_const_iterator
{
   return RAM16K.end();
}

constexpr auto 
Hack::Memory::ram_cend()       const noexcept -> RAM_const_iterator
{
   return RAM16K.cend();
}

constexpr auto 
Hack::Memory::screen_begin()   const noexcept -> Screen_const_iterator
{
   return Screen.begin();
}

constexpr auto 
Hack::Memory::screen_cbegin()  const noexcept -> Screen_const_iterator
{
   return Screen.cbegin();
}

constexpr auto 
Hack::Memory::screen_end()          noexcept -> Screen_iterator
{
   return Screen.end();
}

constexpr auto 
Hack::Memory::screen_end()     const noexcept -> Screen_const_iterator
{
   return Screen.end();
}

constexpr auto 
Hack::Memory::screen_cend()    const noexcept -> Screen_const_iterator
{
   return Screen.cend();
}

constexpr auto 
Hack::Memory::keyboard()           noexcept  -> reference
{
   return Keyboard;
}

constexpr auto 
Hack::Memory::keyboard()       const noexcept -> const_reference
{
   return Keyboard;
}

constexpr auto 
Hack::Memory::clear()                noexcept -> void
{
  clear_ram();
  clear_screen();
  clear_keyboard();
}

constexpr auto 
Hack::Memory::clear_screen()         noexcept -> void
{
   Screen.fill( 0 );
}

constexpr auto 
Hack::Memory::clear_ram()            noexcept -> void
{
   RAM16K.fill( 0 );
}

constexpr auto 
Hack::Memory::clear_keyboard()       noexcept -> void
{
   Keyboard = 0;
}

#endif      // HACK_EMULATOR_2024_03_11_MEMORY_H