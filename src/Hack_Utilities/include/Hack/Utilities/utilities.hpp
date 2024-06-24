/**
 * @file    utilities.hpp
 * @author  William Weston
 * @brief   Utility functions for Hack Emulator
 * @version 0.1
 * @date    2024-03-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef HACK_EMULATOR_PROJECT_2024_03_11_UTILITIES_HPP
#define HACK_EMULATOR_PROJECT_2024_03_11_UTILITIES_HPP

// ------------------------------------------ includes --------------------------------------------

#include <bit>                // for bit_cast
#include <charconv>           // for from_chars, from_chars_result
#include <cstddef>            // for size_t
#include <cstdint>            // for uint16_t, int16_t
#include <optional>           // for optional, nullopt
#include <string>             // for string, char_traits
#include <string_view>        // for string_view, hash
#include <system_error>       // for errc


// ------------------------------------------------------------------------------------------------
// --------------------------------------------- API ----------------------------------------------
// ------------------------------------------------------------------------------------------------


namespace Hack::Utils
{

// ------------------------------------------------------------------------------------------------
// Conversion Utilities ---------------------------------------------------------------------------

// reinterpret bit patterns
constexpr auto signed_to_unsigned_16( std::int16_t value )    -> std::uint16_t;
constexpr auto unsigned_to_signed_16( std::uint16_t value )   -> std::int16_t;

// convert a 16-bit binary string to its uint16_t equivalent
constexpr auto binary_to_uint16( std::string_view binary16 )  -> std::optional<std::uint16_t>;
// convert 16-bit 2's compplement binary string to its int16_t equivalent
constexpr auto binary_to_int16( std::string_view binary16 )   -> std::optional<std::int16_t>;

// base 10 integer string
constexpr auto to_uint16_t( std::string_view value )          -> std::optional<std::uint16_t>;
constexpr auto to_int16_t( std::string_view value )           -> std::optional<std::int16_t>;

// convert string respresenting a positive base10 number into binary 16 string
auto to_binary16_string( std::string_view positive_base10 )   -> std::optional<std::string>;

// return std::string() on error
auto to_binary16_string( std::uint16_t value ) -> std::string;
auto to_binary16_string( std::int16_t value )  -> std::string;

// returns a four character hex string or string() on error
auto to_hex4_string( std::uint16_t value ) -> std::string;

// return hex value of the 2's complement binary representation of the int16_t
auto to_hex4_string( std::int16_t value )  -> std::string;


// ------------------------------------------------------------------------------------------------
// cctype Wrapper Utilities ----------------------------------------------------------------------

auto is_alnum( char ch )  -> bool;
auto is_alpha( char ch )  -> bool;
auto is_lower( char ch )  -> bool;
auto is_upper( char ch )  -> bool;
auto is_digit( char ch )  -> bool;
auto is_xdigit( char ch ) -> bool;
auto is_cntrl( char ch )  -> bool;
auto is_graph( char ch )  -> bool;
auto is_space( char ch )  -> bool;
auto is_blank( char ch )  -> bool;
auto is_print( char ch )  -> bool;
auto is_punct( char ch )  -> bool;

auto to_lower( char ch )  -> char;
auto to_upper( char ch )  -> char;


// ------------------------------------------------------------------------------------------------
// Hack Code Utilities ----------------------------------------------------------------------------

// A-instruction - Binary:     0vvvvvvvvvvvvvvv     •  vv ... v = 15-bit value of xxx
constexpr auto is_a_instruction( std::uint16_t instruction ) -> bool;

// if the 'a' bit is set then the y input to the ALU comes from the M register (RAM_[A]), else from the A register
constexpr auto is_a_bit_set( std::uint16_t instruction )     -> bool;

// does the string represent a 16-bit binary number
constexpr auto is_binary16_string( std::string_view str )    -> bool;


// ------------------------------------------------------------------------------------------------
// File Utilities ---------------------------------------------------------------------------------


// ------------------------------------------------------------------------------------------------
// Hash Functions ---------------------------------------------------------------------------------

struct string_hash final
{
    using hash_type      = std::hash<std::string_view>;
    using is_transparent = void;
 
    std::size_t operator()( const char* str )        const { return hash_type{}( str ); }
    std::size_t operator()( std::string_view str )   const { return hash_type{}( str ); }
    std::size_t operator()( std::string const& str ) const { return hash_type{}( str ); }
};


// ------------------------------------------------------------------------------------------------
// Numeric ----------------------------------------------------------------------------------------

auto number_of_digits( std::uint16_t value )   -> int;



}  // namespace Hack::Utils -----------------------------------------------------------------------


// ------------------------------------------------------------------------------------------------
// ---------------------------------------- Implementation ----------------------------------------


constexpr auto 
Hack::Utils::signed_to_unsigned_16( std::int16_t value )  -> std::uint16_t
{
   return std::bit_cast<std::uint16_t>( value );
}

constexpr auto 
Hack::Utils::unsigned_to_signed_16( std::uint16_t value ) -> std::int16_t
{
   return std::bit_cast<std::int16_t>( value );
}


constexpr auto 
Hack::Utils::binary_to_uint16( std::string_view binary16 )  -> std::optional<std::uint16_t>
{
   if ( binary16.size() != 16 )   
   {
      return std::nullopt;
   }

   auto result           = std::uint16_t{};
   auto const* const end = binary16.data() + binary16.size();

   auto [ptr, ec] = std::from_chars( binary16.data(), end, result, 2 );

   if ( ec == std::errc() && ptr == end )
   {
      return result;
   }

   return std::nullopt;
}

constexpr auto 
Hack::Utils::binary_to_int16( std::string_view binary16 )   -> std::optional<std::int16_t>
{
   auto const result = binary_to_uint16( binary16 );

   if ( !result ) 
   {
      return std::nullopt;
   }
   
   return unsigned_to_signed_16( *result );
}

constexpr auto 
Hack::Utils::to_uint16_t( std::string_view value )          -> std::optional<std::uint16_t>
{
   auto result = std::uint16_t{};
   auto const* const end = value.data() + value.size();

   auto [ptr, ec] = std::from_chars( value.data(), end, result, 10 );

   if ( ec == std::errc() && ptr == end )
   {
      return result;
   }

   return std::nullopt;
}

constexpr auto 
Hack::Utils::to_int16_t( std::string_view value )           -> std::optional<std::int16_t>
{
   auto result = std::int16_t{};
   auto const* const end = value.data() + value.size();

   auto [ptr, ec] = std::from_chars( value.data(), end, result, 10 );

   if ( ec == std::errc() && ptr == end )
   {
      return result;
   }

   return std::nullopt;
}

// Hack Code Utilities ----------------------------------------------------------------------------

constexpr auto 
Hack::Utils::is_a_instruction( std::uint16_t instruction ) -> bool
{
  constexpr auto mask = std::uint16_t{ 0b1000'0000'0000'0000 };

  return !( instruction & mask );
}

constexpr auto 
Hack::Utils::is_a_bit_set( std::uint16_t instruction )     -> bool
{
   // 1111'1100'0000'0000
   // 5432'1098'7654'3210
   // 111a'cccc'ccdd'djjj
   constexpr auto mask = std::uint16_t{ 0b0001'0000'0000'0000 };

   return instruction & mask;
}


constexpr auto 
Hack::Utils::is_binary16_string( std::string_view str )    -> bool
{
   return ( str.size() == 16 ) && 
          ( str.find_first_not_of( "01" ) == std::string_view::npos );
}

#endif      // HACK_EMULATOR_PROJECT_2024_03_11_UTILITIES_HPP