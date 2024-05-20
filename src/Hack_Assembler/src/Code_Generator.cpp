/**
 * @file    Code_Generator.cpp
 * @author  William Weston
 * @brief   Generates binary code for Hack Computer
 * @version 0.1
 * @date    2024-03-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Code_Generator.h"

#include <optional>
#include <string>
#include <string_view>


auto 
Hack::Code_Generator::dest( std::string_view op_code ) const -> std::optional<std::string>
{
   if ( auto const iter = dest_table_.find( op_code ); iter != dest_table_.end() )
   {
      return iter->second;
   }

   return std:: nullopt;
}

auto 
Hack::Code_Generator::comp( std::string_view op_code ) const -> std::optional<std::string>
{
   if ( auto const iter = comp_table_.find( op_code ); iter != comp_table_.end() )
   {
      return iter->second;
   }

   return std:: nullopt;
}

auto 
Hack::Code_Generator::jump( std::string_view op_code ) const -> std::optional<std::string>
{
   if ( auto const iter = jump_table_.find( op_code ); iter != jump_table_.end() )
   {
      return iter->second;
   }

   return std:: nullopt;
}