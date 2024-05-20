cmake_minimum_required( VERSION 3.29 )

project( ImGuiFileDialog_project )

add_library( ImGuiFileDialog )
add_library( ImGuiFileDialog::ImGuiFileDialog ALIAS ImGuiFileDialog )

target_sources( ImGuiFileDialog
   PRIVATE 
      include/ImGuiFileDialog/ImGuiFileDialog.h
      ImGuiFileDialog.cpp
      include/ImGuiFileDialog/ImGuiFileDialogConfig.h
)

target_include_directories( ImGuiFileDialog 
   PUBLIC
     "${CMAKE_CURRENT_SOURCE_DIR}/include"
   PRIVATE 
      "${CMAKE_CURRENT_SOURCE_DIR}"
      "${CMAKE_CURRENT_SOURCE_DIR}/include/ImGuiFileDialog"
)

set ( ImGuiFileDialog_PUBLIC_HEADERS
   "include/ImGuiFileDialog/ImGuiFileDialog.h"
   "include/ImGuiFileDialog/ImGuiFileDialogConfig.h"
)

set_target_properties( ImGuiFileDialog
   PROPERTIES 
      PUBLIC_HEADER ImGuiFileDialog_PUBLIC_HEADERS
)

if( UNIX )
   target_compile_options( ImGuiFileDialog
      PUBLIC 
         -Wno-unknown-pragmas
   )
endif()

target_link_libraries( ImGuiFileDialog
   PUBLIC  
      imgui::imgui
)