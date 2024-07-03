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

#include "Definitions.h"                     // for Format
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
   update_alu_  = true;
   SDL_Log( "instruction: %u, d: %u, a/m: %u, alu out: %u", instruction_, d_input_, am_input_, alu_output_ );
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
   if ( update_alu_ ) { update_alu(); }

   with_StyleVar( ImGuiStyleVar_SeparatorTextAlign, ImVec2{ 0.5, 0.5 } )
      ImGui::SeparatorText( "Hack Computer ALU" );

   ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
   
   ImGui::Columns( 3 );

   {
      ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

      ImGui::Indent( 20.0f );

      auto d_input_string = EMULATOR::Utils::to_string( fmt, d_input_ );

      ImGui::TextUnformatted( "D Input:" );
      ImGui::InputText( "##d input", &d_input_string, ImGuiInputTextFlags_ReadOnly );

      d_location_ = { ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), std::move( d_input_string ) };

      ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
      ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
      
      auto am_input_string = EMULATOR::Utils::to_string( fmt, am_input_ );
      
      ImGui::TextUnformatted( "M/A Input:" );
      ImGui::InputText( "##m/a input", &am_input_string, ImGuiInputTextFlags_ReadOnly );

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
   }

   ImGui::NextColumn();

   {
      ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
      ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

      auto alu_output_string = EMULATOR::Utils::to_string( fmt, alu_output_ );

      ImGui::Indent( 20.0f );
      ImGui::TextUnformatted( "ALU Output:" );
      
      ImGui::InputText( "##alu_ouput", &alu_output_string, ImGuiInputTextFlags_ReadOnly );

      alu_location_ = { ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), std::move( alu_output_string ) };
   }

}


auto 
Hack::ALUDisplay::update_alu() -> void
{
   static constexpr auto DELAY = 2;
   
   static auto delay_count = 0; 

   if ( delay_count < DELAY )
   { 
      ++delay_count;
      return;
   }

   delay_count = 0;
   update_alu_ = false;
   alu_output_ = computer_.ALU_output();
}