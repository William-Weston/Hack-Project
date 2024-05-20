#include <cstdlib>         


#include <imgui.h>
#include <imgui_impl_sdlrenderer2.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include <ImGuiSugar/imgui_sugar.hpp>
#include <SDL.h>

#include <Hack/hack_config.hpp>

#include <iostream>

auto main() -> int
{
   using namespace Hack::cmake;

   std::cout << "Project Name:        " << project_name << '\n';
   std::cout << "Project Version:     " << project_version << '\n';
   std::cout << "Project Description: " << project_description << '\n';
   
   return EXIT_SUCCESS;
}