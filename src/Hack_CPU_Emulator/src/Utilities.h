/**
 * @file    Utilities.h
 * @author  William Weston (wjtWeston@protonmail.com)
 * @brief   Utilities for Hack_CPU_Emulator
 * @version 0.1
 * @date    2024-05-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef HACK_EMULATOR_PROJECT_2024_05_14_HACK_CPU_EMULATOR_UTILITIES_H
#define HACK_EMULATOR_PROJECT_2024_05_14_HACK_CPU_EMULATOR_UTILITIES_H

#include "Hack/Utilities/exceptions.hpp"     // for Exception

#include <imgui.h>
#include <cstdint>                           // for uint16_t
#include <string>                            // for string
#include <type_traits>                       // for is_integral_v, is_signed_v, remove_cvref_t
#include <vector>                            // for vector



namespace Hack::EMULATOR::Utils
{

struct FileError
{
   std::string filename;
};

using file_error                 = Hack::Utils::Exception<FileError>;
using unsupported_filetype_error = Hack::Utils::Exception<void*>;

auto open_hack_file( std::string const& path ) -> std::vector<std::uint16_t>;
auto open_asm_file( std::string const& path )  -> std::vector<std::uint16_t>;


// get the appropriate ImGuiDataType_ for a given integral type 
template <typename T>
consteval auto get_ImGuiDataType() -> ImGuiDataType_
{
   using U = std::remove_cvref_t<T>;
   if constexpr ( std::is_integral_v<U> )
   {
      if constexpr ( std::is_signed_v<U> )
      {
         if constexpr ( sizeof( U ) * 8 == 8 )
         {
            return ImGuiDataType_S8;
         }
         else if constexpr ( sizeof( U ) * 8 == 16 )
         {
            return ImGuiDataType_S16;
         }
         else if constexpr ( sizeof( U ) * 8 == 32 )
         {
            return ImGuiDataType_S32;
         }
         else if constexpr ( sizeof( U ) * 8 == 64 )
         {
            return ImGuiDataType_S64;
         }
         else
         {
            static_assert( false, "Value type not supported" );
         }
      }
      else
      {
         if constexpr ( sizeof( U ) * 8 == 8 )
         {
            return ImGuiDataType_U8;
         }
         else if constexpr ( sizeof( U ) * 8 == 16 )
         {
            return ImGuiDataType_U16;
         }
         else if constexpr ( sizeof( U ) * 8 == 32 )
         {
            return ImGuiDataType_U32;
         }
         else if constexpr ( sizeof( U ) * 8 == 64 )
         {
            return ImGuiDataType_U64;
         }
         else
         {
            static_assert( false, "Value type not supported" );
         }
      }
   }
   else
   {
      static_assert( false, "Must be an integral type" );
   }

   return ImGuiDataType_COUNT;
}

template <typename Type>
consteval auto get_hex_format() -> char const *
{
   using T = std::remove_cvref_t<Type>;

   static_assert( std::is_integral_v<T>, "T must be an integral type" );

   if constexpr ( sizeof( T ) == 1 )
   {
      return "%02X";
   }
   else if constexpr ( sizeof( T ) == 2 )
   {
      return "%04X";
   }
   else if constexpr ( sizeof( T ) == 4 )
   {
      return "%08X";
   }
   else if constexpr ( sizeof( T ) == 8 )
   {
      return "%016X";
   }
   else 
   {
      static_assert( false, "Value type not supported" );
   }

   return "";
}

}        // Hack::Emulator::Utils


#endif      // HACK_EMULATOR_PROJECT_2024_05_14_HACK_CPU_EMULATOR_UTILITIES_H