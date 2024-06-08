    

#include <Hack/buildinfo.h>            // for BuildInfo
#include <cstdlib>                     // for EXIT_SUCCESS
#include <iostream>                    // for char_traits, basic_ostream, operator<<
#include <string_view>                 // for operator<<


auto main() -> int
{
   using namespace Hack;

   std::cout << "Project Name:        " << BuildInfo::project_name << '\n';
   std::cout << "Project Version:     " << BuildInfo::project_version << '\n';
   std::cout << "Project Description: " << BuildInfo::project_description << '\n';
   std::cout << "Timestamp:           " << BuildInfo::timestamp << '\n';
   std::cout << "Commit SHA:          " << BuildInfo::commit_sha << '\n';     

   // int x;

   // std::cout << x << '\n';        
   return EXIT_SUCCESS;
}