/**
 * @file exceptions.hpp
 * @author William Weston (wjtWeston@protonmail.com)
 * @brief
 * @version 0.1
 * @date 2024-05-14
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef HACK_EMULATOR_PROJECT_2024_05_14_EXCEPTIONS_HPP
#define HACK_EMULATOR_PROJECT_2024_05_14_EXCEPTIONS_HPP

#include <cstddef>
#include <ostream>
#include <source_location>
// #include <stacktrace>
#include <string>
#include <utility>               // move

namespace Hack::Utils
{

template <typename DATA_T>
class Exception
{
public:
   Exception( char const* str, 
              DATA_T const& data,
              std::source_location const& loc = std::source_location::current() )
            //   std::stacktrace trace = std::stacktrace::current() )
      : err_str_( str ),
        data_( data ),
        location_{ loc }
      //   backtrace_{ trace }
   {}

   Exception( std::string str, 
              DATA_T data, 
              std::source_location const& loc = std::source_location::current() )
            //   std::stacktrace trace = std::stacktrace::current() )
      : err_str_( std::move( str ) ), 
        data_( std::move( data ) ), 
        location_{ loc }
      //   backtrace_{ trace }
   {}

   Exception( Exception const& )                             = default;
   Exception( Exception && )          noexcept               = default;
   auto operator=( Exception const& )          -> Exception& = default;
   auto operator=( Exception&& )      noexcept -> Exception& = default;

   auto what()                 -> std::string&                { return err_str_; }
   auto what()  const noexcept -> std::string const&          { return err_str_; }

   auto data()                 -> DATA_T&                     { return data_; }
   auto data()  const noexcept -> DATA_T const&               { return data_; }
   
   auto where() const          -> std::source_location const& { return location_; }
   // auto stack() const          -> std::stacktrace const&      { return backtrace_; }

private:
   std::string                err_str_;
   DATA_T                     data_;
   std::source_location const location_;
   // std::stacktrace const      backtrace_;
};


struct ParseErrorData
{
   std::string text;
   std::size_t line_no;
};

using parse_error = Exception<ParseErrorData>;

} // namespace Hack::Utils

inline std::ostream& operator<<( std::ostream& os, std::source_location const& location )
{
   os << "Source Location:\n"
      << "\tfile:     " << location.file_name() << " (" << location.line() << ":" << location.column()  << ")\n" 
      << "\tfunction: " << location.function_name() 
      << "\n";
   return os;
}


// not support yet
// std::ostream& operator<<( std::ostream& os, std::stacktrace const& backtrace )
// {
//    for ( auto iter = backtrace.begin(); iter != ( backtrace.end() - 3 ); ++iter )
//    {
//       os << iter->source_file() 
//          << "(" 
//         << iter->source_line() 
//          << ") : " 
//         << iter->description()       
//          << "\n";
//    }
//    return os;
// }


#endif // HACK_EMULATOR_PROJECT_2024_05_14_EXCEPTIONS_HPP