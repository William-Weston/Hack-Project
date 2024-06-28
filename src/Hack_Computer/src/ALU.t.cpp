/**
 * @file    ALU.t.cpp
 * @author  William Weston
 * @brief   Test file for ALU.h
 * @version 0.1
 * @date    2024-03-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "ALU.h"
#include "Hack/Utilities/utilities.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE( "Computer: ALU Tests" )
{
   using namespace Hack;
   using namespace Hack::Utils;

   SECTION( "out = 0" )
   {
      constexpr auto result = ALU( ALU_in{ .x=42, .y=23, .zx=true, .nx=false, .zy=true, .ny=false, .f=true, .no=false } );

      REQUIRE( result.out == 0 );
      REQUIRE( result.zr == true );
      REQUIRE( result.ng == false );
   }

   SECTION( "out = 1" )
   {
      constexpr auto result = ALU( ALU_in{ .x=42, .y=23, .zx=true, .nx=true, .zy=true, .ny=true, .f=true, .no=true } );

      REQUIRE( result.out == 1 );
      REQUIRE( result.zr == false );
      REQUIRE( result.ng == false );
   }

   SECTION( "out = -1" )
   {
      constexpr auto result = ALU( ALU_in{ .x=42, .y=23, .zx=true, .nx=true, .zy=true, .ny=false, .f=true, .no=false } );

      auto const signed_out = unsigned_to_signed_16( result.out );

      REQUIRE( signed_out == -1 );
      REQUIRE( result.zr == false );
      REQUIRE( result.ng == true );
   }

   SECTION( "out = x" )
   {  
      constexpr auto x      = 42u;
      constexpr auto result = ALU( ALU_in{ .x=x, .y=23, .zx=false, .nx=false, .zy=true, .ny=true, .f=false, .no=false } );

      REQUIRE( result.out == x );
      REQUIRE( result.zr == false );
      REQUIRE( result.ng == false );
   }

   SECTION( "out = y" )
   {
      constexpr auto y      = 23u;
      constexpr auto result = ALU( ALU_in{ .x=42, .y=y, .zx=true, .nx=true, .zy=false, .ny=false, .f=false, .no=false } );

      REQUIRE( result.out == y );
      REQUIRE( result.zr == false );
      REQUIRE( result.ng == false );
   }

   SECTION( "out = ~x" )
   {
      SECTION( " x = 42" )
      {
         constexpr auto x      = std::uint16_t( 42 );
         constexpr auto not_x  = std::uint16_t(~x);
         constexpr auto result = ALU( ALU_in{ .x=x, .y=23, .zx=false, .nx=false, .zy=true, .ny=true, .f=false, .no=true } );

         REQUIRE( result.out == not_x );
         REQUIRE( result.zr == false );
         REQUIRE( result.ng == true );
      }
   }

   SECTION( "out = ~y" )
   {
      SECTION( "y = 23" )
      {
         constexpr auto y      = std::uint16_t( 23 );
         constexpr auto not_y  = std::uint16_t(~y);
         constexpr auto result = ALU( ALU_in{ .x=42, .y=y, .zx=true, .nx=true, .zy=false, .ny=false, .f=false, .no=true } );

         REQUIRE( result.out == not_y );
         REQUIRE( result.zr == false );
         REQUIRE( result.ng == true );
      }

      SECTION( "y = 0b1111'0000'0000'1111" )
      {
         constexpr auto y      = std::uint16_t( 0b1111'0000'0000'1111 );
         constexpr auto not_y  = std::uint16_t(~y);
         constexpr auto result = ALU( ALU_in{ .x=42, .y=y, .zx=true, .nx=true, .zy=false, .ny=false, .f=false, .no=true } );

         REQUIRE( result.out == not_y );
         REQUIRE( result.zr == false );
         REQUIRE( result.ng == false );
      }
   }

   SECTION( "out = -x" )
   {
      constexpr auto x      = 42u;
      constexpr auto neg_x  = std::int16_t( -x );
      constexpr auto result = ALU( ALU_in{ .x=x, .y=23, .zx=false, .nx=false, .zy=true, .ny=true, .f=true, .no=true } );

      auto const signed_out = unsigned_to_signed_16( result.out );

      REQUIRE( signed_out == neg_x );
      REQUIRE( result.zr == false );
      REQUIRE( result.ng == true );
   }

   SECTION( "out = -y" )
   {
      constexpr auto y      = 23u;
      constexpr auto neg_y  = std::int16_t( -y );
      constexpr auto result = ALU( ALU_in{ .x=42, .y=y, .zx=true, .nx=true, .zy=false, .ny=false, .f=true, .no=true } );

      auto const signed_out = unsigned_to_signed_16( result.out );

      REQUIRE( signed_out == neg_y );
      REQUIRE( result.zr == false );
      REQUIRE( result.ng == true );
   }

   SECTION( "out = x + 1" )
   {
      constexpr auto x      = 42u;
      constexpr auto result = ALU( ALU_in{ .x=x, .y=23, .zx=false, .nx=true, .zy=true, .ny=true, .f=true, .no=true } );

      REQUIRE( result.out == x + 1);
      REQUIRE( result.zr == false );
      REQUIRE( result.ng == false );
   }

   SECTION( "out = y + 1" )
   {
      constexpr auto y      = 23u;
      constexpr auto result = ALU( ALU_in{ .x=42, .y=y, .zx=true, .nx=true, .zy=false, .ny=true, .f=true, .no=true } );

      REQUIRE( result.out == y + 1);
      REQUIRE( result.zr == false );
      REQUIRE( result.ng == false );
   }

   SECTION( "out = x - 1" )
   {
      SECTION( "x > 0 " )
      {
         constexpr auto x      = 42u;
         constexpr auto result = ALU( ALU_in{ .x=x, .y=23, .zx=false, .nx=false, .zy=true, .ny=true, .f=true, .no=false } );

         REQUIRE( result.out == x - 1 );
         REQUIRE( result.zr == false );
         REQUIRE( result.ng == false );
      }

      SECTION( "x == 1" )
      {
         constexpr auto x      = 1u;
         constexpr auto result = ALU( ALU_in{ .x=x, .y=23, .zx=false, .nx=false, .zy=true, .ny=true, .f=true, .no=false } );

         REQUIRE( result.out == 0 );
         REQUIRE( result.zr == true );
         REQUIRE( result.ng == false );
      }

      SECTION( "x == 0" )
      {
         constexpr auto result = ALU( ALU_in{ .x=0, .y=23, .zx=false, .nx=false, .zy=true, .ny=true, .f=true, .no=false } );

         auto const signed_out = unsigned_to_signed_16( result.out );

         REQUIRE( signed_out == -1 );
         REQUIRE( result.zr == false );
         REQUIRE( result.ng == true );
      }


      SECTION( "x < 0" )
      {
         constexpr auto x      = std::uint16_t( 0b1111'1111'0000'0001 );   // -255
         constexpr auto result = ALU( ALU_in{ .x=x, .y=23, .zx=false, .nx=false, .zy=true, .ny=true, .f=true, .no=false } );

         auto const signed_out = unsigned_to_signed_16( result.out );

         REQUIRE( signed_out == -256 );
         REQUIRE( result.zr == false );
         REQUIRE( result.ng == true );
      }
   }

   SECTION( "out = y - 1" )
   {
      SECTION( "y > 0 " )
      {
         constexpr auto y      = 42u;
         constexpr auto result = ALU( ALU_in{ .x=42, .y=y, .zx=true, .nx=true, .zy=false, .ny=false, .f=true, .no=false } );

         REQUIRE( result.out == y - 1 );
         REQUIRE( result.zr == false );
         REQUIRE( result.ng == false );
      }

      SECTION( "y == 1" )
      {
         constexpr auto y      = 1u;
         constexpr auto result = ALU( ALU_in{ .x=42, .y=y, .zx=true, .nx=true, .zy=false, .ny=false, .f=true, .no=false } );

         REQUIRE( result.out == 0 );
         REQUIRE( result.zr == true );
         REQUIRE( result.ng == false );
      }

      SECTION( "y == 0" )
      {
         constexpr auto result = ALU( ALU_in{ .x=42, .y=0, .zx=true, .nx=true, .zy=false, .ny=false, .f=true, .no=false } );

         auto const signed_out = unsigned_to_signed_16( result.out );

         REQUIRE( signed_out == -1 );
         REQUIRE( result.zr == false );
         REQUIRE( result.ng == true );
      }


      SECTION( "y < 0" )
      {
         constexpr auto y      = std::uint16_t( 0b1111'1111'0000'0001 );   // -255
         constexpr auto result = ALU( ALU_in{ .x=42, .y=y, .zx=true, .nx=true, .zy=false, .ny=false, .f=true, .no=false } );

         auto const signed_out = unsigned_to_signed_16( result.out );

         REQUIRE( signed_out == -256 );
         REQUIRE( result.zr == false );
         REQUIRE( result.ng == true );
      }
   }

   SECTION( "out = x + y" )
   {
      constexpr auto x = 42u;
      constexpr auto y = 23u;
      constexpr auto result = ALU( ALU_in{ .x=x, .y=y, .zx=false, .nx=false, .zy=false, .ny=false, .f=true, .no=false } );

      REQUIRE( result.out == x + y );
      REQUIRE( result.zr == false );
      REQUIRE( result.ng == false );
   }

   SECTION( "out = x - y" )
   {
      SECTION( "x > y" )
      {
         constexpr auto x = 42u;
         constexpr auto y = 23u;
         constexpr auto result = ALU( ALU_in{ .x=x, .y=y, .zx=false, .nx=true, .zy=false, .ny=false, .f=true, .no=true } );

         REQUIRE( result.out == x - y );
         REQUIRE( result.zr == false );
         REQUIRE( result.ng == false );
      }

      SECTION( "x < y" )
      {
         constexpr auto x        = 23u;
         constexpr auto y        = 42u;
         constexpr auto expected = std::int16_t( x - y );
         constexpr auto result   = ALU( ALU_in{ .x=x, .y=y, .zx=false, .nx=true, .zy=false, .ny=false, .f=true, .no=true } );

         auto const signed_out = unsigned_to_signed_16( result.out );

         REQUIRE( signed_out == expected );
         REQUIRE( result.zr == false );
         REQUIRE( result.ng == true );
      }

      SECTION( "x == y" )
      {
         constexpr auto x = 42u;
         constexpr auto y = 42u;
         constexpr auto result = ALU( ALU_in{ .x=x, .y=y, .zx=false, .nx=true, .zy=false, .ny=false, .f=true, .no=true } );

         REQUIRE( result.out == 0 );
         REQUIRE( result.zr == true );
         REQUIRE( result.ng == false );
      }
   }

   SECTION( "out = y - x" )
   {
      SECTION( "y > x" )
      {
         constexpr auto x = 23u;
         constexpr auto y = 42u;
         constexpr auto result = ALU( ALU_in{ .x=x, .y=y, .zx=false, .nx=false, .zy=false, .ny=true, .f=true, .no=true } );

         REQUIRE( result.out == y - x );
         REQUIRE( result.zr == false );
         REQUIRE( result.ng == false );
      }

      SECTION( "y < x" )
      {
         constexpr auto x        = 42u;
         constexpr auto y        = 23u;
         constexpr auto expected = std::int16_t( y - x );
         constexpr auto result   = ALU( ALU_in{ .x=x, .y=y, .zx=false, .nx=false, .zy=false, .ny=true, .f=true, .no=true } );

         auto const signed_out = unsigned_to_signed_16( result.out );

         REQUIRE( signed_out == expected );
         REQUIRE( result.zr == false );
         REQUIRE( result.ng == true );
      }

      SECTION( "x == y" )
      {
         constexpr auto x = 42u;
         constexpr auto y = 42u;
         constexpr auto result = ALU( ALU_in{ .x=x, .y=y, .zx=false, .nx=false, .zy=false, .ny=true, .f=true, .no=true } );

         REQUIRE( result.out == 0 );
         REQUIRE( result.zr == true );
         REQUIRE( result.ng == false );
      }
   }

   SECTION( "out = x & y" )
   {
      SECTION( "x & y == 0" )
      {
         constexpr auto x        = 0b1111'1111'0000'0000;
         constexpr auto y        = 0b0000'0000'1111'1111;
         constexpr auto expected = 0;

         constexpr auto result   = ALU( ALU_in{ .x=x, .y=y, .zx=false, .nx=false, .zy=false, .ny=false, .f=false, .no=false } );

         REQUIRE( result.out == expected );
         REQUIRE( result.zr == true );
         REQUIRE( result.ng == false );
      }

      SECTION( "x & y != 0" )
      {
         constexpr auto x        = 0b1111'0000'0000'1111;
         constexpr auto y        = 0b0000'0000'0000'1111;
         constexpr auto expected = 0b0000'0000'0000'1111;

         constexpr auto result   = ALU( ALU_in{ .x=x, .y=y, .zx=false, .nx=false, .zy=false, .ny=false, .f=false, .no=false } );

         REQUIRE( result.out == expected );
         REQUIRE( result.zr == false );
         REQUIRE( result.ng == false );
      }
      
   }

   SECTION( "out = x | y" )
   {
      constexpr auto x        = 0b1111'0000'0000'0000;
      constexpr auto y        = 0b0000'0000'0000'1111;
      constexpr auto expected = 0b1111'0000'0000'1111;

      constexpr auto result   = ALU( ALU_in{ .x=x, .y=y, .zx=false, .nx=true, .zy=false, .ny=true, .f=false, .no=true } );

      REQUIRE( result.out == expected );
      REQUIRE( result.zr == false );
      REQUIRE( result.ng == true );
   }
}