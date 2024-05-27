/**
 * @file    Disassembler.cpp
 * @author  William Weston
 * @brief   Source file for Disassembler.h
 * @version 0.1
 * @date    2024-03-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Disassembler.h"

#include "Hack/Utilities/utilities.hpp"  // for to_binary16_string

#include <cassert>                       // for assert
#include <charconv>                      // for from_chars, from_chars_result
#include <cstdint>                       // for int16_t, uint16_t
#include <optional>                      // for optional, nullopt
#include <string_view>                   // for string_view, hash, operator==
#include <system_error>                  // for errc
#include <utility>                       // for pair, move



auto 
Hack::Disassembler::disassemble( std::string_view binary ) const -> std::optional<std::string>
{
   static constexpr auto instruction_size = 16;

   if ( binary.size() != instruction_size )
   {
      return std::nullopt;
   }

   if ( binary.front() == '0' )
   {
      return a_instruction( binary );
   }

   return c_instruction( binary );
}

auto 
Hack::Disassembler::disassemble( std::uint16_t instruction ) const -> std::optional<std::string>
{
   return disassemble( Hack::Utils::to_binary16_string( instruction ) );
}

/*
   Symbolic:   @xxx
   Binary:     0vvvvvvvvvvvvvvv
*/
auto 
Hack::Disassembler::a_instruction( std::string_view binary ) const -> std::optional<std::string>
{
   assert( binary.size() == 16 );

   binary.remove_prefix( 1 );      // remove opt-code

   auto const* const end = binary.data() + binary.size();
   auto value            = std::int16_t{} ;

   if ( auto const [ptr, ec] = std::from_chars( binary.data(), end, value, 2 );
        ec == std::errc() && ptr == end )
   {
      return std::make_optional<std::string>( "@" + std::to_string( value ) );
   }

   return std::nullopt;
}

/*
   Symbolic:   dest=comp;jump
   Binary:     111accccccdddjjj
*/
auto 
Hack::Disassembler::c_instruction( std::string_view binary ) const -> std::optional<std::string>
{
   assert( binary.size() == 16 );

   // 111 acccccc ddd jjj
   auto const c = std::string_view( binary.begin() + 3,  binary.begin() + 10 );
   auto const d = std::string_view( binary.begin() + 10, binary.begin() + 13 );
   auto const j = std::string_view( binary.begin() + 13, binary.begin() + 16 );

   auto const comp_iter = comp_table_.find( c );
   auto const dest_iter = dest_table_.find( d ); 
   auto const jump_iter = jump_table_.find( j );
   
   if ( comp_iter == comp_table_.end() || dest_iter == dest_table_.end() || jump_iter == jump_table_.end() )
   {
      return std::nullopt;
   }

   auto result = std::string();

   if ( auto const dest = dest_iter->second; !dest.empty() )
   {
      result += dest + "=";
   }

   result += comp_iter->second;

   if ( auto const jump = jump_iter-> second; !jump.empty() )
   {
      result += ";" + jump;
   }

   return std::make_optional<std::string>( std::move( result ) );
}