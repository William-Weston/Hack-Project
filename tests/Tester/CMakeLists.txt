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

option( HACK_TESTER_ENABLE_CLANGTIDY "Enable clang-tidy" OFF )
option( HACK_TESTER_ENABLE_CPPCHECK "Enable cppcheck" OFF )
option( HACK_TESTER_ENABLE_IWYU "Enable iwyu" ON )
option( HACK_TESTER_ENABLE_LWYU "Enable link whay you use" ON )

include( StaticAnalyzers )

EnableSanitizers( main 
   HACK_TESTER_ENABLE_CLANGTIDY
   HACK_TESTER_ENABLE_CPPCHECK
   HACK_TESTER_ENABLE_IWYU
   HACK_TESTER_ENABLE_LWYU
)