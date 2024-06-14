/**
 * @file utilities.cpp
 * @author William Weston (wjtWeston@protonmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "utilities.hpp"

#include <array>           // for array
#include <cctype>          // for islower, isalnum, isalpha, isblank, iscntrl, isg...
#include <charconv>        // for to_chars, to_chars_result
#include <cstdint>         // for int16_t, uint16_t
#include <optional>        // for optional, nullopt
#include <string>          // for string, allocator
#include <utility>         // for move

// ------------------------------------------------------------------------------------------------
namespace   // helper function declarations -------------------------------------------------------
{

}


// Conversion Utilities ---------------------------------------------------------------------------

auto 
Hack::Utils::to_binary16_string( std::string_view positive_base10 ) -> std::optional<std::string>
{
   auto const number = to_uint16_t( positive_base10 );

   if ( ! number )
   {
      return std::nullopt;
   }

   auto result = to_binary16_string( *number );

   if ( result.empty() )
   {
      return std::nullopt;
   }

   return { std::move( result ) };        // move result into the optional
   
}

auto 
Hack::Utils::to_binary16_string( std::uint16_t value ) -> std::string
{
   static constexpr auto bits16 = 16;

   auto buffer = std::array<char, bits16>{};

   if ( auto [ptr,ec] = std::to_chars( buffer.data(), buffer.data() + buffer.size(), value, 2 );
      ec == std::errc() )
   {
      auto const size = ptr - buffer.data();
      auto const fill = static_cast<std::string::size_type>( 16 - size );
      auto str        = std::string( fill, '0' );
      str.insert( str.cend(), buffer.data(), ptr );
      return str;
   }

   return {};
}

auto 
Hack::Utils::to_binary16_string( std::int16_t value )   -> std::string
{
   auto const u_value = signed_to_unsigned_16( value );

   return to_binary16_string( u_value );
}


auto 
Hack::Utils::to_hex4_string( std::uint16_t value ) -> std::string
{
   static constexpr auto hex = 16;

   auto buffer = std::array<char, 4>{};

   if ( auto [ptr,ec] = std::to_chars( buffer.data(), buffer.data() + buffer.size(), value, hex );
      ec == std::errc() )
   {
      auto const size = ptr - buffer.data();
      auto const fill = static_cast<std::string::size_type>( 4 - size );
      auto str = std::string( fill, '0' );
      str.insert( str.cend(), buffer.data(), ptr );
      return str;
   }
   return {};
}

auto 
Hack::Utils::to_hex4_string( std::int16_t value )  -> std::string
{
   auto const u_value = signed_to_unsigned_16( value );

   return to_hex4_string( u_value );
}


// cctype Wrapper Utilities ----------------------------------------------------------------------

auto 
Hack::Utils::is_alnum( char ch )  -> bool
{
   return std::isalnum( static_cast<unsigned char>( ch ) );
}

auto 
Hack::Utils::is_alpha( char ch )  -> bool
{
   return std::isalpha( static_cast<unsigned char>( ch ) );
}

auto 
Hack::Utils::is_lower( char ch )  -> bool
{
   return std::islower( static_cast<unsigned char>( ch ) );
}

auto 
Hack::Utils::is_upper( char ch )  -> bool
{
   return std::isupper( static_cast<unsigned char>( ch ) );
}

auto 
Hack::Utils::is_digit( char ch )  -> bool
{
   return std::islower( static_cast<unsigned char>( ch ) );
}

auto 
Hack::Utils::is_xdigit( char ch ) -> bool
{
   return std::isxdigit( static_cast<unsigned char>( ch ) );
}

auto 
Hack::Utils::is_cntrl( char ch )  -> bool
{
   return std::iscntrl( static_cast<unsigned char>( ch ) );
}

auto 
Hack::Utils::is_graph( char ch )  -> bool
{
   return std::isgraph( static_cast<unsigned char>( ch ) );
}

auto 
Hack::Utils::is_space( char ch )  -> bool
{
   return std::isspace( static_cast<unsigned char>( ch ) );
}

auto 
Hack::Utils::is_blank( char ch )  -> bool
{
   return std::isblank( static_cast<unsigned char>( ch ) );
}

auto 
Hack::Utils::is_print( char ch )  -> bool
{
   return std::isprint( static_cast<unsigned char>( ch ) );
}

auto 
Hack::Utils::is_punct( char ch )  -> bool
{
   return std::ispunct( static_cast<unsigned char>( ch ) );
}

auto 
Hack::Utils::to_lower( char ch ) -> char
{
   return static_cast<char>( std::tolower( static_cast<unsigned char>( ch ) ) );
}

auto 
Hack::Utils::to_upper( char ch ) -> char
{
   return static_cast<char>( std::toupper( static_cast<unsigned char>( ch ) ) );
}

// Hack Code Utilities ----------------------------------------------------------------------------


// File Utilities ---------------------------------------------------------------------------------

// Numeric Utilities -------------------------------------------------------------------------------

auto 
Hack::Utils::number_of_digits( std::uint16_t value )   -> int
{
   if ( value < 10 )       return 1;
   if ( value < 100 )      return 2;
   if ( value < 1'000 )    return 3;
   if ( value < 10'000 )   return 4;
   
   return 5;
}