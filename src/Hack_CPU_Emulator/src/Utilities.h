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

#include "Hack/Utilities/exceptions.hpp"

#include <cstdint>         // uint16_t
#include <cstddef>         // size_t

#include <string> 

#include <vector>  


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

}        // Hack::Emulator::Utils


#endif      // HACK_EMULATOR_PROJECT_2024_05_14_HACK_CPU_EMULATOR_UTILITIES_H