/**
 * @file Memory.t.cpp
 * @author William Weston (wjtWeston@protonmail.com)
 * @brief Test file for Memory.h
 * @version 0.1
 * @date 2024-05-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Hack/Memory.h"

#include <catch2/catch_all.hpp>

#include <stdexcept>          // out_of_range

TEST_CASE( "Computer: operator[]" )
{
   using namespace Hack;

   SECTION( "expect exception when accessing out of range" )
   {
      auto mem = Memory();

      REQUIRE_THROWS_AS( mem[Memory::address_space], std::out_of_range );
   }

   SECTION( "ram access" )
   {
      auto mem = Memory();
      mem[0] = 1;

      REQUIRE( *mem.ram_begin() == 1 );
   }

   SECTION( "screen access" )
   {
      auto mem = Memory();
      mem[Memory::screen_start_address] = 1;

      REQUIRE( *mem.screen_begin() == 1 );
   }

   SECTION( "keyboard access" )
   {
      auto mem = Memory();
      mem[Memory::keyboard_address] = 1;

      REQUIRE( mem.keyboard() == 1 );
   }
}
