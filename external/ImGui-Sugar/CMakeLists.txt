cmake_minimum_required( VERSION 3.29 )

project( ImGuiSugar )

add_library( ImGuiSugar INTERFACE )
add_library( ImGuiSugar::ImGuiSugar ALIAS ImGuiSugar )

target_sources( ImGuiSugar
   PUBLIC 
      include/ImGuiSugar/imgui_sugar.hpp
)

target_include_directories( ImGuiSugar
   INTERFACE 
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)

target_link_libraries( ImGuiSugar
   INTERFACE 
      imgui::imgui
)