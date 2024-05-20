/**
 * @file    CPU.t.cpp
 * @author  William Weston
 * @brief   Test file for CPU.h
 * @version 0.1
 * @date    2024-03-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Hack/CPU.h"

#include "Hack/Memory.h"

#include <catch2/catch_all.hpp>
#include <cstdint>


TEST_CASE( "Register Accessors" )
{
   using namespace Hack;
   auto ram = Memory();
   auto cpu = CPU( ram );

   SECTION( "A Register" )
   {
      cpu.A_Register() = 23;

      REQUIRE( cpu.A_Register() == 23 );
   }

   SECTION( "D Register" )
   {
      cpu.D_Register() = 23;

      REQUIRE( cpu.D_Register() == 23 );
   }

   SECTION( "M Register" )
   {
      cpu.M_Register() = 23;

      REQUIRE( cpu.M_Register() == 23 );

      SECTION( "Change M Register location by setting A Register" )
      {
         cpu.A_Register() = 42;
         cpu.M_Register() = 11;

         REQUIRE( cpu.M_Register() == 11 );

         cpu.A_Register() = 99;
         cpu.M_Register() = 7;

         REQUIRE( cpu.M_Register() == 7 );
      }
   }
}


TEST_CASE( "Simple Register Tests: put 1 or -1 into each register" )
{
   using namespace Hack;
   auto ram = Memory();
   auto cpu = CPU( ram );

   SECTION( "put 1 in ram[0]")
   {
      auto const instruction = std::uint16_t{ 0b1111'1111'1100'1000 };  // M=1
      auto const next        = cpu.execute_instruction( instruction );

      REQUIRE( next == 1 );
      REQUIRE( ram[0] == 1 );
   }

   SECTION( "put -1 in ram[0]")
   {
      auto const instruction = std::uint16_t{ 0b1111'1101'0000'1000 };  // M=-1
      auto const next        = cpu.execute_instruction( instruction );

      REQUIRE( next == 1 );
      REQUIRE( ram[0] == 0b1111'1111'1111'1111 );
   }

   SECTION( "put 1 in A Register" )
   {
      auto const instruction = std::uint16_t{ 0b1111'1111'1110'0000 };  // A=1
      auto const next        = cpu.execute_instruction( instruction );

      REQUIRE( next == 1 );
      REQUIRE( cpu.A_Register() == 1 );
   }

   SECTION( "put -1 in A Register")
   {
      auto const instruction = std::uint16_t{ 0b1111'1101'0010'0000 };  // A=-1
      auto const next        = cpu.execute_instruction( instruction );

      REQUIRE( next == 1 );
      REQUIRE( cpu.A_Register() == 0b1111'1111'1111'1111 );
   }

   SECTION( "put 1 in D Register" )
   {
      auto const instruction = std::uint16_t{ 0b1111'1111'1101'0000 };  // D=1
      auto const next        = cpu.execute_instruction( instruction );

      REQUIRE( next == 1 );
      REQUIRE( cpu.D_Register() == 1 );
   }

   SECTION( "put -1 in D Register")
   {
      auto const instruction = std::uint16_t{ 0b1111'1101'0001'0000 };  //D=-1
      auto const next        = cpu.execute_instruction( instruction );

      REQUIRE( next == 1 );
      REQUIRE( cpu.D_Register() == std::uint16_t{ 0b1111'1111'1111'1111 } );
   }
}


TEST_CASE( "Set registers to zero" )
{
   using namespace Hack;
   auto ram = Memory();
   auto cpu = CPU( ram );

   cpu.A_Register() = 23;
   cpu.D_Register() = 42;
   cpu.M_Register() = 11;

   SECTION( "when 'a' bit == 0")
   {
      SECTION( "A Register" )
      {
         auto const instruction = std::uint16_t{ 0b1110'1010'1010'0000 };  // A=0
         auto const next        = cpu.execute_instruction( instruction );

         REQUIRE( next == 1 );
         REQUIRE( cpu.A_Register() == 0 );
      }

      SECTION( "D Register" )
      {
         auto const instruction = std::uint16_t{ 0b1110'1010'1001'0000 };  // D=0
         auto const next        = cpu.execute_instruction( instruction );

         REQUIRE( next == 1 );
         REQUIRE( cpu.D_Register() == 0 );
      }

      SECTION( "M Register" )
      {
         auto const instruction = std::uint16_t{ 0b1110'1010'1000'1000 };  // M=0
         auto const next        = cpu.execute_instruction( instruction );

         REQUIRE( next == 1 );
         REQUIRE( cpu.M_Register() == 0 );
      }
   }

   SECTION( "when 'a' bit == 1")
   {
      SECTION( "A Register" )
      {
         auto const instruction = std::uint16_t{ 0b1111'1010'1010'0000 };  
         auto const next        = cpu.execute_instruction( instruction );

         REQUIRE( next == 1 );
         REQUIRE( cpu.A_Register() == 0 );
      }

      SECTION( "D Register" )
      {
         auto const instruction = std::uint16_t{ 0b1111'1010'1001'0000 };
         auto const next        = cpu.execute_instruction( instruction );

         REQUIRE( next == 1 );
         REQUIRE( cpu.D_Register() == 0 );
      }

      SECTION( "M Register" )
      {
         auto const instruction = std::uint16_t{ 0b1111'1010'1000'1000 };
         auto const next        = cpu.execute_instruction( instruction );

         REQUIRE( next == 1 );
         REQUIRE( cpu.M_Register() == 0 );
      }
   }
}


TEST_CASE( "Compute the binary negation of the value in each register" )
{
   using namespace Hack;
   auto ram = Memory();
   auto cpu = CPU( ram );

   SECTION( "Register A" )
   {
      SECTION( "A=!A" )
      {
         auto const value    = std::uint16_t{ 0b0000'0000'1111'1111 };
         auto const expected = static_cast<std::uint16_t>(~value);
         cpu.A_Register()    = value;

         auto const instruction = std::uint16_t{ 0b1110'1100'0110'0000 };  // A=!A
         auto const next        = cpu.execute_instruction( instruction );

         REQUIRE( next == 1 );
         REQUIRE( cpu.A_Register() == expected );
      }

      SECTION( "AMD=!A" )
      {
         auto const value    = std::uint16_t{ 0b0000'0000'1111'1111 };
         auto const expected = static_cast<std::uint16_t>(~value);
         cpu.A_Register()    = value;

         auto const instruction = std::uint16_t{ 0b1110'1100'0111'1000 };  // AMD=!A
         auto const next        = cpu.execute_instruction( instruction );

         CHECK( next == 1 );
         CHECK( cpu.A_Register() == expected );
         CHECK( cpu.D_Register() == expected );
         CHECK( ram[value] == expected );
      }
   }
   
   SECTION( "Register D" )
   {
      SECTION( "D=!D" )
      {
         auto const value    = std::uint16_t{ 0b0000'0000'1111'1111 };
         auto const expected = static_cast<std::uint16_t>(~value);
         cpu.D_Register()    = value;

         auto const instruction = std::uint16_t{ 0b1110'0011'0101'0000 };  // D=!D
         auto const next        = cpu.execute_instruction( instruction );

         REQUIRE( next == 1 );
         REQUIRE( cpu.D_Register() == expected );
      }

      SECTION( "AMD=!D" )
      {
         auto const value    = std::uint16_t{ 0b0000'0000'1111'1111 };
         auto const expected = static_cast<std::uint16_t>(~value);
         cpu.D_Register()    = value;

         auto const instruction = std::uint16_t{ 0b1110'0011'0111'1000  };  // AMD=!D
         auto const next        = cpu.execute_instruction( instruction );

         CHECK( next == 1 );
         CHECK( cpu.D_Register() == expected );
         CHECK( cpu.A_Register() == expected );
         CHECK( ram[0] == expected );                                    // A Register = 0 before instruction executed

      }
   }

   SECTION( "Register M" )
   {
      SECTION( "M=!M" )
      {
         auto const value    = std::uint16_t{ 0b0000'0000'1111'1111 };
         auto const expected = static_cast<std::uint16_t>(~value);
         cpu.M_Register()    = value;                                      // RAM[0] = value

         auto const instruction = std::uint16_t{ 0b1111'1100'0100'1000 };  // M=!M
         auto const next        = cpu.execute_instruction( instruction );

         
         REQUIRE( next == 1 );
         REQUIRE( cpu.M_Register() == expected );
      }

      SECTION( "AMD=!M" )
      {
         auto const value    = std::uint16_t{ 0b0000'0000'1111'1111 };
         auto const expected = static_cast<std::uint16_t>(~value);
         cpu.M_Register()    = value;

         auto const instruction = std::uint16_t{ 0b1111'1100'0111'1000 };  // AMD=!M
         auto const next        = cpu.execute_instruction( instruction );

         CHECK( next == 1 );
         CHECK( cpu.D_Register() == expected );
         CHECK( cpu.A_Register() == expected );
         CHECK( ram[0] == expected );                                    // A Register = 0 before instruction executed

      }
   }
}


TEST_CASE( "Arithmatic operations" )
{
   using namespace Hack;
   auto ram = Memory();
   auto cpu = CPU( ram );

   cpu.A_Register() = 42;
   cpu.D_Register() = 23;

   SECTION( "M=D+A")
   {
      auto const instruction = std::uint16_t{ 0b1110'0000'1000'1000 };  // M=D+A
      auto const next        = cpu.execute_instruction( instruction );

      REQUIRE( next == 1 );
      REQUIRE( ram[42] == 42 + 23 );
   }

   SECTION( "AMD=D+M" )
   {
      cpu.M_Register()       = 7;
      auto const instruction = std::uint16_t{ 0b1111'0000'1011'1000 };  // AMD=D+M
      auto const next        = cpu.execute_instruction( instruction );
      auto const expected    = 23 + 7;

      CHECK( next == 1 );
      CHECK( cpu.A_Register() == expected );
      CHECK( cpu.D_Register() == expected );
      CHECK( ram[42] == expected );
   }
}


TEST_CASE( "Jump Operations" )
{
   using namespace Hack;
   auto ram = Memory();
   auto cpu = CPU( ram );

   SECTION( "unconditional jump" )
   {
      cpu.A_Register() = 42;

      auto const instruction = std::uint16_t{ 0b1111'1010'1000'0111 };  // 0;JMP
      auto const next        = cpu.execute_instruction( instruction );

      REQUIRE( next == 42 );
      CHECK( cpu.A_Register() == 42 );
      CHECK( cpu.D_Register() == 0 );
      CHECK( cpu.M_Register() == 0 );
   }
   
   SECTION( "jump if D == 0, D;JEQ" )
   {
      SECTION( "D == 0" )
      {
         cpu.A_Register() = 42;

         auto const instruction = std::uint16_t{ 0b1111'0011'0000'0010 };  // D;JEQ
         auto const next        = cpu.execute_instruction( instruction );

         REQUIRE( next == 42 );
         CHECK( cpu.A_Register() == 42 );
         CHECK( cpu.D_Register() == 0 );
         CHECK( cpu.M_Register() == 0 );
      }

      SECTION( "D != 0" )
      {
         cpu.A_Register() = 42;
         cpu.D_Register() = 7;
         auto const instruction = std::uint16_t{ 0b1111'0011'0000'0010 };  // D;JEQ
         auto const next        = cpu.execute_instruction( instruction );

         REQUIRE( next == 1 );
         CHECK( cpu.A_Register() == 42 );
         CHECK( cpu.D_Register() == 7 );
         CHECK( cpu.M_Register() == 0 );
      }
   }

   SECTION( "jump if D > 0, D;JGT" )
   {
      SECTION( "D > 0 ")
      {
         cpu.A_Register() = 42;
         cpu.D_Register() = 1;

         auto const instruction = std::uint16_t{ 0b1111'0011'0000'0001 };  // D;JGT
         auto const next        = cpu.execute_instruction( instruction );

         REQUIRE( next == 42 );
         CHECK( cpu.A_Register() == 42 );
         CHECK( cpu.D_Register() == 1 );
         CHECK( cpu.M_Register() == 0 );
      }

      SECTION( " D < 0 " )
      {
         cpu.A_Register() = 42;
         cpu.D_Register() = std::uint16_t{ 0b1000'0000'0000'0000 };

         auto const instruction = std::uint16_t{ 0b1111'0011'0000'0001 };  // D;JGT
         auto const next        = cpu.execute_instruction( instruction );

         REQUIRE( next == 1 );
         CHECK( cpu.A_Register() == 42 );
         CHECK( cpu.D_Register() == std::uint16_t{ 0b1000'0000'0000'0000 } );
         CHECK( cpu.M_Register() == 0 );
      }

      SECTION( "D == 0 " )
      {
         cpu.A_Register() = 42;
         cpu.D_Register() = 0;

         auto const instruction = std::uint16_t{ 0b1111'0011'0000'0001 };  // D;JGT
         auto const next        = cpu.execute_instruction( instruction );

         REQUIRE( next == 1 );
         CHECK( cpu.A_Register() == 42 );
         CHECK( cpu.D_Register() == 0 );
         CHECK( cpu.M_Register() == 0 );
      }
   }

   SECTION( "jump if D < 0, D;JLT" )
   {
      SECTION( "D < 0" )
      {
         cpu.A_Register() = 42;
         cpu.D_Register() = std::uint16_t{ 0b1111'1111'1101'0110 };

         auto const instruction = std::uint16_t{ 0b1111'0011'0000'0100 };  // D;JLT
         auto const next        = cpu.execute_instruction( instruction );

         REQUIRE( next == 42 );
         CHECK( cpu.A_Register() == 42 );
         CHECK( cpu.D_Register() == std::uint16_t{ 0b1111'1111'1101'0110 } );
         CHECK( cpu.M_Register() == 0 );
      }

      SECTION( "D == 0" )
      {
         cpu.A_Register() = 42;
         cpu.D_Register() = 0;

         auto const instruction = std::uint16_t{ 0b1111'0011'0000'0100 };  // D;JLT
         auto const next        = cpu.execute_instruction( instruction );

         REQUIRE( next == 1 );
         CHECK( cpu.A_Register() == 42 );
         CHECK( cpu.D_Register() == 0 );
         CHECK( cpu.M_Register() == 0 );
      }

      SECTION( "D > 0" )
      {
         cpu.A_Register() = 42;
         cpu.D_Register() = 1;

         auto const instruction = std::uint16_t{ 0b1111'0011'0000'0100 };  // D;JLT
         auto const next        = cpu.execute_instruction( instruction );

         REQUIRE( next == 1 );
         CHECK( cpu.A_Register() == 42 );
         CHECK( cpu.D_Register() == 1 );
         CHECK( cpu.M_Register() == 0 );
      }
   }
}