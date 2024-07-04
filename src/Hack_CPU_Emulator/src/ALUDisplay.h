/**
 * @file    ALUDisplay.h
 * @author  William Weston (wjtWeston@protonmail.com)
 * @brief   Display the internals of the Hack ALU
 * @version 0.1
 * @date    2024-07-03
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef HACK_PROJECT_INCLUDE_2024_07_03_ALUDISPLAY_H
#define HACK_PROJECT_INCLUDE_2024_07_03_ALUDISPLAY_H

#include "Definitions.h"                  // for DataLocation, Format

#include "Hack/Computer.h"                // for Computer


namespace Hack
{

class ALUDisplay final
{
public:
   explicit ALUDisplay( Computer& computer );

   // update GUI elements
   auto update( Format fmt )         -> void;

   // update GUI to reflect the last instruction executed
   auto next_instruction()           -> void;

   // clear all inputs and output
   auto clear()                      -> void;

   auto d_location()           const -> DataLocation;
   auto am_location()          const -> DataLocation;
   auto out_location()         const -> DataLocation;

private:
   using word_t = Hack::Computer::word_t;

   Computer&     computer_;
   DataLocation  d_location_   {};
   DataLocation  am_location_  {};
   DataLocation  alu_location_ {};
   word_t        instruction_  {};
   word_t        am_input_     {};
   word_t        d_input_      {};
   word_t        alu_output_   {};

   auto do_update( Format fmt ) -> void;
   auto update_inputs()         -> void;
};

}  // namespace Hack

#endif // HACK_PROJECT_INCLUDE_2024_07_03_ALUDISPLAY_H