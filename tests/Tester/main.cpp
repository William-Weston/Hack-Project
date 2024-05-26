#include <cstdlib>         


#include <imgui.h>
#include <imgui_impl_sdlrenderer2.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include <ImGuiSugar/imgui_sugar.hpp>
#include <SDL.h>

#include <Hack/buildinfo.h>

#include <iostream>

auto main() -> int
{
   using namespace Hack;

   std::cout << "Project Name:        " << BuildInfo::project_name << '\n';
   std::cout << "Project Version:     " << BuildInfo::project_version << '\n';
   std::cout << "Project Description: " << BuildInfo::project_description << '\n';
   std::cout << "Timestamp:           " << BuildInfo::timestamp << '\n';
   std::cout << "Commit SHA:          " << BuildInfo::commit_sha << '\n';             
   return EXIT_SUCCESS;
}