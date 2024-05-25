cmake_minimum_required( VERSION 3.29 )

project( test )
add_executable( main )

target_sources( main 
   PRIVATE 
      main.cpp 
)

target_link_libraries( main 
   PRIVATE 
      imgui::imgui
      ImGuiFileDialog::ImGuiFileDialog
      ImGuiSugar::ImGuiSugar
      Hack::hack_config
      Hack::project_warnings
)

include( StaticAnalyzers )
AddIWYU( main )
AddLWYU( main )
