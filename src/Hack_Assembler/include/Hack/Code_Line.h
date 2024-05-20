/**
 * @file    Code_Line.h
 * @author  William Weston
 * @brief   A line of assembly code with line  number used for error messages
 * @version 0.1
 * @date    2024-03-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef HACK_2024_03_21_CODE_LINE_H
#define HACK_2024_03_21_CODE_LINE_H

#include <cstddef>      // size_t
#include <string>

// represents a line containing an instruction in the .asm file
struct Code_Line final
{
   std::string instruction = std::string();
   std::string text        = std::string();
   std::size_t line_no     = 0;
};

#endif      // HACK_2024_03_21_CODE_LINE_H