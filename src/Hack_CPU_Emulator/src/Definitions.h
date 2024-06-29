/**
 * @file    Definitions.h
 * @author  William Weston (wjtWeston@protonmail.com)
 * @brief   Definitions for Hack CPU Emulator
 * @version 0.1
 * @date    2024-05-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef  HACK_PROJECT_2024_05_17_DEFINITIONS_H
#define  HACK_PROJECT_2024_05_17_DEFINITIONS_H

#include <imgui.h>        // for ImVec2

#include <string>

namespace Hack
{

struct UserError
{
   std::string description;
   std::string msg;
   bool        activate;
};



struct DataLocation
{
   ImVec2      top_left{};
   ImVec2      bottom_right{};
   std::string data{};
};

enum class Format
{
   DECIMAL,
   HEX,
   BINARY,
   ASM,
   NONE,
};

struct main_options
{
   bool   track_pc;
   Format format;
};

inline constexpr auto ITEM_WIDTH = 130.0;

}     // namespace Hack 

#endif         // HACK_PROJECT_2024_05_17_DEFINITIONS_H