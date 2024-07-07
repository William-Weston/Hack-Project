/**
 * @file    ALUDisplay.cpp
 * @author  William Weston (wjtWeston@protonmail.com)
 * @brief   Source file for ALUDisplay.h
 * @version 0.1
 * @date    2024-07-03
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "ALUDisplay.h"

#include "Definitions.h"                     // for Format, ITEM_WIDTH
#include "Utilities.h"                       // for to_string

#include "Hack/Disassembler.h"               // for disassemble
#include "Hack/Utilities/utilities.hpp"      // for is_a_instruction, to_binary16_string

#include "ImGuiSugar/imgui_sugar.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <SDL_log.h>

#include <utility>                           // for move


Hack::ALUDisplay::ALUDisplay( Computer& computer )
   : computer_{ computer }
{}

auto 
Hack::ALUDisplay::update( Format fmt ) -> void
{
   // Ensure enough space to draw table columns, else imgui will crash
   if ( ImGui::GetContentRegionAvail().x > 50.0f )
      do_update( fmt );
}

auto 
Hack::ALUDisplay::next_instruction() -> void
{
   update_inputs();
}


auto 
Hack::ALUDisplay::clear() -> void
{
   instruction_ = word_t{};
   am_input_    = word_t{};
   d_input_     = word_t{};
   alu_output_  = word_t{};
}


auto 
Hack::ALUDisplay::format()  const -> Format
{
   return format_;
}


auto 
Hack::ALUDisplay::comp_location() const -> DataLocation
{
   return comp_location_;
}


auto 
Hack::ALUDisplay::d_location() const -> DataLocation
{
   return d_location_;
}


auto 
Hack::ALUDisplay::am_location() const -> DataLocation
{
   return am_location_;
}


auto 
Hack::ALUDisplay::out_location() const -> DataLocation
{
   return alu_location_;
}


auto 
Hack::ALUDisplay::do_update( Format fmt ) -> void
{
   format_ = fmt;

   with_StyleVar( ImGuiStyleVar_SeparatorTextAlign, ImVec2{ 0.5, 0.5 } )
      ImGui::SeparatorText( "Hack Computer ALU" );

   ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
   
   auto const instruction_string = Hack::Utils::to_binary16_string( instruction_ );
   ImGui::Text( "Instruction: %s", instruction_string.data() );

   ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();


   ImGui::Columns( 3 );

   {
      ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

      ImGui::Indent( 20.0f );

      auto d_input_string = EMULATOR::Utils::to_string( fmt, d_input_ );

      ImGui::TextUnformatted( "D Input:" );
      ImGui::SetNextItemWidth( ITEM_WIDTH );
      ImGui::InputText( "##d input", &d_input_string, ImGuiInputTextFlags_ReadOnly );

      d_location_ = { ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), std::move( d_input_string ) };

      ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
      ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
      
      auto am_input_string = EMULATOR::Utils::to_string( fmt, am_input_ );
      
      ImGui::TextUnformatted( "A/M Input:" );
      ImGui::SetNextItemWidth( ITEM_WIDTH );
      ImGui::InputText( "##a/m input", &am_input_string, ImGuiInputTextFlags_ReadOnly );

      am_location_ = { ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), std::move( am_input_string ) };
   }

   ImGui::NextColumn();

   {
      ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

      GUI::Utils::CentreTextUnformatted( " --- Computation --- " );

      ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
      ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

      auto const binary   = Hack::Utils::to_binary16_string( instruction_ );
      auto const comp_opt = Disassembler::computation( binary );

      if ( comp_opt )
         GUI::Utils::CentreTextUnformatted( comp_opt->data() );
      else
         GUI::Utils::CentreTextUnformatted( "---" );

      auto const size     = ImGui::GetItemRectSize();
      auto const top_left = ImGui::GetItemRectMin();
      auto const pos      = ImVec2{ top_left.x - ( ( ITEM_WIDTH - size.x ) / 2.0f ), top_left.y - ImGui::GetStyle().FramePadding.y };
      
      comp_location_ = { pos, {}, "" };  // DataLocation::bottom_right&  DataLocation::data is not needed, the values are never used
   }

   ImGui::NextColumn();

   {
      ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
      ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

      auto alu_output_string = EMULATOR::Utils::to_string( fmt, alu_output_ );

      ImGui::Indent( 20.0f );
      ImGui::TextUnformatted( "ALU Output:" );
      
      ImGui::SetNextItemWidth( ITEM_WIDTH );
      ImGui::InputText( "##alu_ouput", &alu_output_string, ImGuiInputTextFlags_ReadOnly );

      alu_location_ = { ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), std::move( alu_output_string ) };
   }

}


auto 
Hack::ALUDisplay::update_inputs() -> void
{
   auto const instruction = computer_.ROM().at( computer_.pc() );

   if ( Utils::is_a_instruction( instruction ) )
      return;


   if ( Utils::is_a_bit_set( instruction ) )
   {
      am_input_ = computer_.M_Register();
   }
   else
   {
      am_input_ = computer_.A_Register();
   }

   d_input_     = computer_.D_Register();
   instruction_ = instruction;
   alu_output_  = Computer::evaluate( d_input_, am_input_, instruction_ );
}

