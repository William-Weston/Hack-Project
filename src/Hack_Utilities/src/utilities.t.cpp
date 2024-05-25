/**
 * @file    utilities.t.cpp
 * @author  William Weston
 * @brief   Test file for utilities.h
 * @version 0.1
 * @date    2024-03-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Hack/Utilities/utilities.hpp"

#include <catch2/catch_all.hpp>
#include <cstdint>
#include <string>


TEST_CASE( "Utilites: signed_to_unsigned_16" )
{
   using namespace Hack::Utils;

   SECTION( "zero, one" )
   {
      SECTION( "zero" )
      {
         constexpr auto value    = std::int16_t( 0 );
         constexpr auto expected = std::uint16_t( 0 );
         constexpr auto result   = signed_to_unsigned_16( value );

         REQUIRE( result == expected );
      }
      
      SECTION( "one" )
      {
         constexpr auto value    = std::int16_t( 1 );
         constexpr auto expected = std::uint16_t( 1 );
         constexpr auto result   = signed_to_unsigned_16( value );

         REQUIRE( result == expected );
      }
   }

   SECTION( "value > 0" )
   {
      constexpr auto value    = std::int16_t( 42 );
      constexpr auto expected = std::uint16_t( 42 );
      constexpr auto result   = signed_to_unsigned_16( value );

      REQUIRE( result == expected );
   }

   SECTION( "value < 0" )
   {
      constexpr auto value    = std::int16_t( 0b1111'1111'1101'0110 );   // -42
      constexpr auto expected = std::uint16_t( 0b1111'1111'1101'0110 );  // 65'494
      constexpr auto result   = signed_to_unsigned_16( value );

      REQUIRE( result == expected );
   }
}


TEST_CASE( "Utilites: unsigned_to_signed_16" )
{
   using namespace Hack::Utils;

   SECTION( "zero, one" )
   {
      SECTION( "zero" )
      {
         constexpr auto value    = std::uint16_t( 0 );
         constexpr auto expected = std::int16_t( 0 );
         constexpr auto result   = unsigned_to_signed_16( value );

         REQUIRE( result == expected );
      }
      
      SECTION( "one" )
      {
         constexpr auto value    = std::uint16_t( 1 );
         constexpr auto expected = std::int16_t( 1 );
         constexpr auto result   = unsigned_to_signed_16( value );

         REQUIRE( result == expected );
      }
   }


   SECTION( "value < std::numeric_limits<std::int16_t>" )
   {
      constexpr auto value    = std::uint16_t( 42 );
      constexpr auto expected = std::int16_t( 42 );
      constexpr auto result   = unsigned_to_signed_16( value );

      REQUIRE( result == expected );
   }

   SECTION( "value > std::numeric_limits<std::int16_t>" )
   {
      constexpr auto value    = std::uint16_t( 0b1111'1111'1101'0110 );  // 65'494
      constexpr auto expected = std::int16_t( 0b1111'1111'1101'0110 );   // -42
      constexpr auto result   = unsigned_to_signed_16( value );

      REQUIRE( result == expected );
   }
}


TEST_CASE( "Utilites: binary_to_uint16" )
{
   using namespace Hack::Utils;

   SECTION( "zero" )
   {
      constexpr auto value    = "0000000000000000";
      constexpr auto expected = std::uint16_t{ 0 };

      constexpr auto result = binary_to_uint16( value );

      REQUIRE( result );
      REQUIRE( expected == *result );
   }

   SECTION( "one" )
   {
      constexpr auto value    = "0000000000000001";
      constexpr auto expected = std::uint16_t{ 1 };

      constexpr auto result = binary_to_uint16( value );

      REQUIRE( result );
      REQUIRE( expected == *result );
   }

   SECTION( "unint_t max" )
   {
      constexpr auto value    = "1111111111111111";
      constexpr auto expected = std::uint16_t{ 65535 };

      constexpr auto result = binary_to_uint16( value );
      
      REQUIRE( result );
      REQUIRE( expected == *result );  
   }

   SECTION( "bad data" )
   {
      SECTION( "more than 16-bit string" )
      {
         constexpr auto value  = "00000000000000000";
         constexpr auto result = binary_to_uint16( value );

         REQUIRE( !result );
      }

      SECTION( "less than 16-bit string" )
      {
         constexpr auto value  = "00000000000000";
         constexpr auto result = binary_to_uint16( value );

         REQUIRE( !result );
      }

      SECTION( "non-binary string" )
      {
         SECTION( "contains a digit other than 1 or 0" )
         {
            constexpr auto value  = "0000000000020000";
            constexpr auto result = binary_to_uint16( value );

            REQUIRE( !result );
         }

         SECTION( "contains a character other than 1 or 0" )
         {
            constexpr auto value  = "000000000000a000";
            constexpr auto result = binary_to_uint16( value );

            REQUIRE( !result );
         }
      }
   }
}


TEST_CASE( "Utilites: binary_to_int16" )
{
   using namespace Hack::Utils;

   SECTION( "zero" )
   {
      constexpr auto value    = "0000000000000000";
      constexpr auto expected = std::int16_t{ 0 };

      constexpr auto result = binary_to_int16( value );

      REQUIRE( result );
      REQUIRE( expected == *result );
   }

   SECTION( "one" )
   {
      constexpr auto value    = "0000000000000001";
      constexpr auto expected = std::int16_t{ 1 };

      constexpr auto result = binary_to_int16( value );

      REQUIRE( result );
      REQUIRE( expected == *result );
   }

   SECTION( "negative one" )
   {
      constexpr auto value    = "1111111111111111";
      constexpr auto expected = std::int16_t{ -1 };

      constexpr auto result = binary_to_int16( value );

      REQUIRE( result );
      REQUIRE( expected == *result );
   }

   SECTION( "unint_t max" )
   {
      constexpr auto value    = "0111111111111111";
      constexpr auto expected = std::int16_t{ 32767 };

      constexpr auto result = binary_to_int16( value );
      
      REQUIRE( result );
      REQUIRE( expected == *result );  
   }

   SECTION( "unint_t min" )
   {
      constexpr auto value    = "1000000000000000";
      constexpr auto expected = std::int16_t{ -32768 };

      constexpr auto result = binary_to_int16( value );
      
      REQUIRE( result );
      REQUIRE( expected == *result );  
   }

   SECTION( "bad data" )
   {
      SECTION( "more than 16-bit string" )
      {
         constexpr auto value  = "00000000000000000";
         constexpr auto result = binary_to_int16( value );

         REQUIRE( !result );
      }

      SECTION( "less than 16-bit string" )
      {
         constexpr auto value  = "00000000000000";
         constexpr auto result = binary_to_int16( value );

         REQUIRE( !result );
      }

      SECTION( "non-binary string" )
      {
         SECTION( "contains a digit other than 1 or 0" )
         {
            constexpr auto value  = "0000000000020000";
            constexpr auto result = binary_to_int16( value );

            REQUIRE( !result );
         }

         SECTION( "contains a character other than 1 or 0" )
         {
            constexpr auto value  = "000000000000a000";
            constexpr auto result = binary_to_int16( value );

            REQUIRE( !result );
         }
      }
   }
}


TEST_CASE( "Utilites: to_uint16_t( std::string_view )" )
{
   using namespace Hack::Utils;

   SECTION( "zero" )
   {
      constexpr auto value    = "0";
      constexpr auto expected = std::uint16_t{ 0 };
      constexpr auto result   = to_uint16_t( value );

      REQUIRE( result );
      REQUIRE( expected == *result );
   }

   SECTION( "one" )
   {
      constexpr auto value    = "1";
      constexpr auto expected = std::uint16_t{ 1 };
      constexpr auto result   = to_uint16_t( value );

      REQUIRE( result );
      REQUIRE( expected == *result );
   }

   SECTION( "uint_t max: 65'535" )
   {
      constexpr auto value    = "65535";
      constexpr auto expected = std::uint16_t{ 65'535 };
      constexpr auto result   = to_uint16_t( value );

      REQUIRE( result );
      REQUIRE( expected == *result );
   }

   SECTION( "bad input" )
   {
      SECTION( "negative number: -1" )
      {
         constexpr auto value  = "-1";
         constexpr auto result = to_uint16_t( value );

         REQUIRE( !result );
      }

      SECTION( "greater than uint16_t max" )
      {
         constexpr auto value  = "65536";
         constexpr auto result = to_uint16_t( value );

         REQUIRE( !result );
      }

      SECTION( "non-numeric character at beginning" )
      {
         constexpr auto value  = "a11";
         constexpr auto result = to_uint16_t( value );

         REQUIRE( !result );
      }

      SECTION( "non-numeric character in middle" )
      {
         constexpr auto value  = "12a3";
         constexpr auto result = to_uint16_t( value );

         REQUIRE( !result );
      }

      SECTION( "non-numeric character at end" )
      {
         constexpr auto value  = "123a";
         constexpr auto result = to_uint16_t( value );

         REQUIRE( !result );
      }

      SECTION( "leading plus sign" )
      {
         constexpr auto value  = "+23";
         constexpr auto result = to_uint16_t( value );

         REQUIRE( !result );
      }
   }
}


TEST_CASE( "Utilites: to_int16_t( std::string_view )" )
{
   using namespace Hack::Utils;

   SECTION( "zero" )
   {
      constexpr auto value    = "0";
      constexpr auto expected = std::int16_t{ 0 };
      constexpr auto result   = to_int16_t( value );

      REQUIRE( result );
      REQUIRE( expected == *result );
   }

   SECTION( "one" )
   {
      constexpr auto value    = "1";
      constexpr auto expected = std::int16_t{ 1 };
      constexpr auto result   = to_int16_t( value );

      REQUIRE( result );
      REQUIRE( expected == *result );
   }

   SECTION( "negative one" )
   {
      constexpr auto value    = "-1";
      constexpr auto expected = std::int16_t{ -1 };
      constexpr auto result   = to_int16_t( value );

      REQUIRE( result );
      REQUIRE( expected == *result );
   }

   
   SECTION( "int_t max: 32'767" )
   {
      constexpr auto value    = "32767";
      constexpr auto expected = std::int16_t{ 32'767 };
      constexpr auto result   = to_int16_t( value );

      REQUIRE( result );
      REQUIRE( expected == *result );
   }

   SECTION( "int_t min: -32'768" )
   {
      constexpr auto value    = "-32768";
      constexpr auto expected = std::int16_t{ -32'768 };
      constexpr auto result   = to_int16_t( value );

      REQUIRE( result );
      REQUIRE( expected == *result );
   }

   SECTION( "bad input" )
   {
      SECTION( "greater than int16_t max" )
      {
         constexpr auto value  = "32768";
         constexpr auto result = to_int16_t( value );

         REQUIRE( !result );
      }

      SECTION( "less than int16_t min" )
      {
         constexpr auto value  = "-32769";
         constexpr auto result = to_int16_t( value );

         REQUIRE( !result );
      }

      SECTION( "non-numeric character at beginning" )
      {
         constexpr auto value  = "a11";
         constexpr auto result = to_int16_t( value );

         REQUIRE( !result );
      }

      SECTION( "non-numeric character in middle" )
      {
         constexpr auto value  = "12a3";
         constexpr auto result = to_int16_t( value );

         REQUIRE( !result );
      }

      SECTION( "non-numeric character at end" )
      {
         constexpr auto value  = "123a";
         constexpr auto result = to_int16_t( value );

         REQUIRE( !result );
      }

      SECTION( "leading plus sign" )
      {
         constexpr auto value  = "+23";
         constexpr auto result = to_int16_t( value );

         REQUIRE( !result );
      }
   }
}


TEST_CASE( "Utilites: to_binary16_string( std::string_view base10 )" )
{
   using namespace Hack::Utils;

   SECTION( "zero" )
   {
      auto const result   = to_binary16_string( "0" );
      auto const expected = "0000000000000000";

      REQUIRE( result );
      REQUIRE( expected == result.value() );

   }

   SECTION( "one" )
   {
      auto const result   = to_binary16_string( "1" );
      auto const expected = "0000000000000001";

      REQUIRE( result );
      REQUIRE( expected == result.value() );
   }

   SECTION( "uint16_t max" )
   {
      auto const result   = to_binary16_string( "65535" );
      auto const expected = "1111111111111111";

      REQUIRE( result );
      REQUIRE( expected == result.value() );
   }

   SECTION( "leading zeros" )
   {
      auto const result   = to_binary16_string( "0000" );
      auto const expected = "0000000000000000";

      REQUIRE( result );
      REQUIRE( expected == result.value() );

   }

   SECTION( "Expect Failure" )
   {
      SECTION( "negative: -1" )
      {
         auto const result = to_binary16_string( "-1" );

         REQUIRE( !result );
      }

      SECTION( "value greater than uint16_t max" )
      {
         auto const result = to_binary16_string( "65536" );

         REQUIRE( !result );
      }

      SECTION( "empty string" )
      {
         auto const result = to_binary16_string( "" );

         REQUIRE( !result );
      }

      SECTION( "trailing spaces" )
      {
         REQUIRE( !to_binary16_string( "112 " ) );
         REQUIRE( !to_binary16_string( "123  " ) );
         REQUIRE( !to_binary16_string( "123   " ) );
      }

      SECTION( "leading spaces" )
      {
         REQUIRE( !to_binary16_string( " 112" ) );
         REQUIRE( !to_binary16_string( "  123" ) );
         REQUIRE( !to_binary16_string( "    123" ) );
      }

      SECTION( "interposed spaces" )
      {
         REQUIRE( !to_binary16_string( "1 12" ) );
         REQUIRE( !to_binary16_string( "12 3" ) );
         REQUIRE( !to_binary16_string( "1 2 3" ) );
      }

      SECTION( "non-numeric strings" )
      {
         REQUIRE( !to_binary16_string( "z112" ) );
         REQUIRE( !to_binary16_string( "12y3" ) );
         REQUIRE( !to_binary16_string( "123o" ) );
      }

      SECTION( "non-base10 strings" )
      {
         REQUIRE( !to_binary16_string( "A9" ) );
         REQUIRE( !to_binary16_string( "FF" ) );
         REQUIRE( !to_binary16_string( "ff" ) );
         REQUIRE( !to_binary16_string( "ABC" ) );
      }
   }
}


TEST_CASE( "Utilites: to_binary16_string( std::uint16_t )" )
{
   using namespace Hack::Utils;

   SECTION( "zero" )
   {
      constexpr auto value    = std::uint16_t{ 0 };
      constexpr auto expected = "0000000000000000";

      auto const result = to_binary16_string( value );

      REQUIRE( expected == result );
   }

   SECTION( "one" )
   {
      constexpr auto value    = std::uint16_t{ 1 };
      constexpr auto expected = "0000000000000001";

      auto const result = to_binary16_string( value );

      REQUIRE( expected == result );
   }

   SECTION( "uint16_t max" )
   {
      constexpr auto value    = std::uint16_t{ 65535 };
      constexpr auto expected = "1111111111111111";

      auto const result = to_binary16_string( value );

      REQUIRE( expected == result );
   }
}


TEST_CASE( "Utilites: to_binary16_string( std::int16_t )" )
{
   using namespace Hack::Utils;

   SECTION( "zero" )
   {
      constexpr auto value    = std::int16_t{ 0 };
      constexpr auto expected = "0000000000000000";

      auto const result = to_binary16_string( value );

      REQUIRE( expected == result );
   }

   SECTION( "one" )
   {
      constexpr auto value    = std::int16_t{ 1 };
      constexpr auto expected = "0000000000000001";

      auto const result = to_binary16_string( value );

      REQUIRE( expected == result );
   }

   SECTION( "negative one" )
   {
      constexpr auto value    = std::int16_t{ -1 };
      constexpr auto expected = "1111111111111111";

      auto const result = to_binary16_string( value );

      REQUIRE( expected == result );
   }

   SECTION( "int16_t max: 32'767" )
   {
      constexpr auto value    = std::int16_t{ 32'767 };
      constexpr auto expected = "0111111111111111";

      auto const result = to_binary16_string( value );

      REQUIRE( expected == result );
   }

   SECTION( "int16_t min: -32'768" )
   {
      constexpr auto value    = std::int16_t{ -32'768 };
      constexpr auto expected = "1000000000000000";

      auto const result = to_binary16_string( value );

      REQUIRE( expected == result );
   }
}


TEST_CASE( "Utilites: to_hex4_string( std::uint16_t )" )
{
   using namespace Hack::Utils;

   SECTION( "zero" )
   {
      constexpr auto value    = std::uint16_t{ 0 };
      constexpr auto expected = "0000";

      auto const result = to_hex4_string( value );

      REQUIRE( expected == result );
   }

   SECTION( "one" )
   {
      constexpr auto value    = std::uint16_t{ 1 };
      constexpr auto expected = "0001";

      auto const result = to_hex4_string( value );

      REQUIRE( expected == result );
   }

   SECTION( "uint16_t max: 65'535" )
   {
      constexpr auto value    = std::uint16_t{ 65'535 };
      constexpr auto expected = "ffff";

      auto const result = to_hex4_string( value );

      REQUIRE( expected == result );
   }
}


TEST_CASE( "Utilites: to_hex4_string( std::int16_t )" )
{
   using namespace Hack::Utils;

   SECTION( "zero" )
   {
      constexpr auto value    = std::int16_t{ 0 };
      constexpr auto expected = "0000";

      auto const result = to_hex4_string( value );

      REQUIRE( expected == result );
   }

   SECTION( "one" )
   {
      constexpr auto value    = std::int16_t{ 1 };
      constexpr auto expected = "0001";

      auto const result = to_hex4_string( value );

      REQUIRE( expected == result );
   }

   SECTION( "negative one" )
   {
      constexpr auto value    = std::int16_t{ -1 };
      constexpr auto expected = "ffff";

      auto const result = to_hex4_string( value );

      REQUIRE( expected == result );
   }

   SECTION( "int16_t max: 32'767" )
   {
      constexpr auto value    = std::int16_t{ 32'767 };
      constexpr auto expected = "7fff";

      auto const result = to_hex4_string( value );

      REQUIRE( expected == result );
   }

   SECTION( "int16_t min: -32'768" )
   {
      constexpr auto value    = std::int16_t{ -32'768 };
      constexpr auto expected = "8000";

      auto const result = to_hex4_string( value );

      REQUIRE( expected == result );
   }
}


TEST_CASE( "Utilites: is_a_instruction" )
{
   using namespace Hack::Utils;

   SECTION( "expect success" )
   {
      constexpr auto instruction = std::uint16_t{ 0b0000'0000'0000'1111 };
      constexpr auto result      = is_a_instruction( instruction );

      REQUIRE( result == true );
   }
   
   SECTION( "expect failure" )
   {
      constexpr auto instruction = std::uint16_t{ 0b1110'0000'0000'1111 };
      constexpr auto result      = is_a_instruction( instruction );

      REQUIRE( result == false );
   }
}


TEST_CASE( "Utilites: is_binary16_string" )
{
   using namespace Hack::Utils;

   SECTION( "expect success" )
   {
      SECTION( "all zeros" )
      {
         REQUIRE( true == is_binary16_string( "0000000000000000" ) );
      }

      SECTION( "all ones" )
      {
         REQUIRE( true == is_binary16_string( "1111111111111111" ) );
      }

      SECTION( "mixed" )
      {
         REQUIRE( true == is_binary16_string( "1010101010101010" ) );
         REQUIRE( true == is_binary16_string( "0101010101010101" ) );
      }

      SECTION( "confidence" )
      {
         REQUIRE( true == is_binary16_string( "0000111100001111" ) );
         REQUIRE( true == is_binary16_string( "1111000011110000" ) );
         REQUIRE( true == is_binary16_string( "1111111100000000" ) );
         REQUIRE( true == is_binary16_string( "0000000011111111" ) );
      }
   }

   SECTION( "expect failure" )
   {
      SECTION( "size != 16" )
      {
         SECTION( "size < 16" )
         {
            REQUIRE( false == is_binary16_string( "0" ) );
            REQUIRE( false == is_binary16_string( "1" ) );
            REQUIRE( false == is_binary16_string( "111111111111111" ) );
            REQUIRE( false == is_binary16_string( "000000000000000" ) );
         }

         SECTION( "size > 16" )
         {
            REQUIRE( false == is_binary16_string( "00000000000000000" ) );
            REQUIRE( false == is_binary16_string( "11111111111111111" ) );
            REQUIRE( false == is_binary16_string( "01010101010101010101" ) );
         }
      }

      SECTION( "contains characters other than '0' & '1'" )
      {
         SECTION( "alphabetic" )
         {
            REQUIRE( false == is_binary16_string( "a000000000000000" ) );
            REQUIRE( false == is_binary16_string( "000000000000000z" ) );
            REQUIRE( false == is_binary16_string( "00000000w0000000" ) );
         }
         
         SECTION( "punctuation" )
         {
            REQUIRE( false == is_binary16_string( "01010101.1010101" ) );
            REQUIRE( false == is_binary16_string( "0101*10101010101" ) );
            REQUIRE( false == is_binary16_string( "0101@10101010101" ) );
         }

         SECTION( "whitespace" )
         {
            REQUIRE( false == is_binary16_string( "0101\n10101010101" ) );
            REQUIRE( false == is_binary16_string( "0101\t10101010101" ) );
            REQUIRE( false == is_binary16_string( "0101 10101010101" ) );
         }
         
      }
   }
}