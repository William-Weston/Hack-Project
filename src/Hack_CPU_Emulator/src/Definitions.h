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

#include <string>

namespace Hack
{

struct UserError
{
   std::string description;
   std::string msg;
   bool        activate;
};

enum class ROMFormat
{
   ASM,
   DECIMAL,
   HEX,
   BINARY,
};

enum class RAMFormat
{
   DECIMAL,
   HEX,
   BINARY,
};

struct main_options
{
   bool      track_pc;
   RAMFormat format;
};


}     // namespace Hack 

#endif         // HACK_PROJECT_2024_05_17_DEFINITIONS_H