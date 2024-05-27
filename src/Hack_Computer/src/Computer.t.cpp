/**
 * @file    Computer.t.cpp
 * @author  William Weston
 * @brief   Test file for Computer.h
 * @version 0.1
 * @date    2024-03-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Hack/Computer.h"

#include <algorithm>          // equal
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <iostream>
#include <iterator>

#include <numeric>      // iota
#include <sstream>      // string_stream
#include <vector>

TEST_CASE( "Computer: Load ROM" )
{
   SECTION( "span overload" )
   {
      using namespace Hack;
      namespace rng = std::ranges;

      auto computer     = Computer();
      auto instructions = std::vector<std::uint16_t>( computer.rom_size() );

      std::iota( instructions.begin(), instructions.end(), 0 );

      computer.load_rom( instructions );

      REQUIRE( rng::equal( instructions, computer.ROM() ) );
   }


   SECTION( "iterator overload" )
   {
      using namespace Hack;
      namespace rng = std::ranges;

      auto computer     = Computer();
      auto instructions = std::vector<std::uint16_t>( computer.rom_size() );

      std::iota( instructions.begin(), instructions.end(), 0 );

      SECTION( "vector iterators" )
      {
         computer.load_rom( instructions.begin(), instructions.end() );

         REQUIRE( rng::equal( instructions, computer.ROM() ) );
      }
      
      SECTION( "ostream_iterators" )
      {
         auto stream = std::stringstream();

         rng::copy( instructions, std::ostream_iterator<std::uint16_t>( stream, " " ) );

         computer.load_rom( std::istream_iterator<std::uint16_t>(stream), std::istream_iterator<std::uint16_t>() );

         REQUIRE( rng::equal( instructions, computer.ROM() ) );
      }
   }
}