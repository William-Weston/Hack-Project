/**
 * @file    DisplayFormats.hpp
 * @author  William Weston (wjtWeston@protonmail.com)
 * @brief   
 * @version 0.1
 * @date    2024-06-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef HACK_PROJECT_INCLUDE_2024_06_28_DISPLAYFORMATS_HPP
#define HACK_PROJECT_INCLUDE_2024_06_28_DISPLAYFORMATS_HPP

#include "Definitions.h"                     // for Format, ITEM_WIDTH
#include "Utilities.h"

#include "Hack/Assembler.h"
#include "Hack/Disassembler.h"
#include "Hack/Utilities/utilities.hpp"      // for binary_to_uint16, to_binary16_string, 

#include <imgui.h>
#include <imgui_stdlib.h>
#include <ImGuiSugar/imgui_sugar.hpp>

#include <cassert>


namespace Hack::Formats
{

auto update_item( auto& item, int index, bool highlight, Format fmt ) -> void;

auto format_asm( auto& item )                              -> void;
auto format_decimal( auto& item )                          -> void;
auto format_hex( auto& item )                              -> void;
auto format_binary( auto& item )                           -> void;
auto format_none( int index )                                         -> void;


// ------------------------------------------------------------------------------------------------

// indent from left to begin printing each item of data
// the indent must be larger than the horizontal screen space taken to print largest index label
// eg:
//     |label   |data
//     |        |
//     |        |
//   left      indent
static constexpr auto INDENT = 60.0f;

auto 
update_item( auto& item, int index, bool highlight, Format fmt ) -> void
{  
   ImGui::AlignTextToFramePadding();
   ImGui::Text( "%d", index );
   ImGui::SameLine( INDENT );

   switch ( fmt )
   {
      case Format::ASM:
      {
         if ( highlight )
         {
            with_StyleColor( ImGuiCol_FrameBg, IM_COL32( 230, 255, 0 , 255 ) )
            with_StyleColor( ImGuiCol_Text, IM_COL32( 0, 0, 0, 255 ) )
               format_asm( item );
         }     
         else
         {
            format_asm( item );
         } 
         
         return;
      }
   
      case Format::DECIMAL:
      {
         if ( highlight )
         {
            with_StyleColor( ImGuiCol_FrameBg, IM_COL32( 230, 255, 0 , 255 ) )
            with_StyleColor( ImGuiCol_Text, IM_COL32( 0, 0, 0, 255 ) )
               format_decimal( item );
         }     
         else
         {
            format_decimal( item );
         } 

         return;
      }

      case Format::HEX:
      {
         if ( highlight )
         {
            with_StyleColor( ImGuiCol_FrameBg, IM_COL32( 230, 255, 0 , 255 ) )
            with_StyleColor( ImGuiCol_Text, IM_COL32( 0, 0, 0, 255 ) )
               format_hex( item );
         }     
         else
         {
            format_hex( item );
         }

         return;
      }

      case Format::BINARY:
      {
         if ( highlight )
         {
            with_StyleColor( ImGuiCol_FrameBg, IM_COL32( 230, 255, 0 , 255 ) )
            with_StyleColor( ImGuiCol_Text, IM_COL32( 0, 0, 0, 255 ) )
               format_binary( item );
         }     
         else
         {
            format_binary( item );
         }

         return;
      }

      case Format::NONE:
      {
         format_none( index );
         return;
      }
   }   
}


auto 
format_asm( auto& item ) -> void
{
   assert( EMULATOR::Utils::get_ImGuiDataType<decltype( item )>() == ImGuiDataType_U16 &&
                  "Disassembler & Assembler only support 16 bit unsigned instruction words" );

   static constexpr auto DEFAULT_STR = "---";

   auto asm_opt         = Disassembler::disassemble( item );
   auto asm_instruction = std::string( DEFAULT_STR );

   if ( asm_opt )
   {
      asm_instruction = *asm_opt;
   }

   
   ImGui::SetNextItemWidth( ITEM_WIDTH );
   ImGui::InputText( "##format_asm", &asm_instruction );

   // validate user input, if any
   if ( asm_instruction == DEFAULT_STR )  { return; }

   static auto const asmblr = Assembler();
   auto const assembled_instruction_opt = asmblr.assemble( asm_instruction );

   if ( !assembled_instruction_opt )   { return; }

   auto const instruction_as_uint16_t_opt = Utils::binary_to_uint16( assembled_instruction_opt.value() );

   if ( !instruction_as_uint16_t_opt ) { return; }

   item = instruction_as_uint16_t_opt.value();
}


auto 
format_decimal( auto& item ) -> void
{
   static constexpr auto data_type = EMULATOR::Utils::get_ImGuiDataType<decltype( item )>();

   ImGui::SetNextItemWidth( ITEM_WIDTH );
   ImGui::InputScalar( "##value", data_type, &item );
}


auto 
format_hex( auto& item )     -> void
{
   static constexpr auto data_type = EMULATOR::Utils::get_ImGuiDataType<decltype( item )>();
   static constexpr auto hex_format = EMULATOR::Utils::get_hex_format<decltype( item )>();

   ImGui::SetNextItemWidth( ITEM_WIDTH );
   ImGui::InputScalar( "##ram", data_type, &item, nullptr, nullptr, hex_format, ImGuiInputTextFlags_CharsUppercase );
}


auto 
format_binary( auto& item )  -> void
{
   static_assert( EMULATOR::Utils::get_ImGuiDataType<decltype( item )>() == ImGuiDataType_U16, 
                  "Only supports 16 bit binary strings" );

   auto binary = Utils::to_binary16_string( item );

   ImGui::SetNextItemWidth( ITEM_WIDTH );
   ImGui::InputText( "##format_asm", &binary );

   if ( auto const value_opt = Utils::binary_to_uint16( binary ); value_opt )
   {
      item = value_opt.value();
   }
}

inline auto 
format_none( int index )  -> void
{
   ImGui::Text( "%d", index );
   ImGui::SameLine( INDENT );
   ImGui::TextUnformatted( " --- " );
}

} // namespace Hack::Formats

#endif  // HACK_PROJECT_INCLUDE_2024_06_28_DISPLAYFORMATS_HPP