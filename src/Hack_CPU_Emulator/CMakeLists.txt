cmake_minimum_required( VERSION 3.29 )

project( Hack_CPU_Emulator
        VERSION        0.1
        DESCRIPTION    "Hack CPU Emulator GUI Project"
        LANGUAGES      CXX
)

# =====================================
# Define Targets
# =====================================

add_executable( Hack_CPU_Emulator_GUI )

target_sources( Hack_CPU_Emulator_GUI
    PRIVATE 
        src/main.cpp
        src/ALUDisplay.h
        src/ALUDisplay.cpp
        src/AnimationHandler.h
        src/AnimationHandler.cpp
        src/animation_t.hpp
        src/DataDisplay.hpp
        src/DatumDisplay.h
        src/DatumDisplay.cpp
        src/Definitions.h
        src/DisplayFormats.hpp
        src/Emulator.h
        src/Emulator.cpp
        src/Keyboard_Handler.h
        src/Keyboard_Handler.cpp
        src/InternalsDisplay.h
        src/InternalsDisplay.cpp
        src/Screen_Texture.h
        src/Screen_Texture.cpp
        src/TextAnimation.h
        src/TextAnimation.cpp
        src/Utilities.h
        src/Utilities.cpp
)

target_include_directories( Hack_CPU_Emulator_GUI 
    PRIVATE 
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
)

target_link_libraries( Hack_CPU_Emulator_GUI
   PRIVATE 
        Hack::project_warnings
        Hack::project_options
        Hack::Assembler
        Hack::Computer
        Hack::Disassembler
        Hack::Utilities
        GUI_Core::GUI_Core
        ImGuiFileDialog::ImGuiFileDialog
        ImGuiSugar::ImGuiSugar
)


# =====================================
# 	OPTIONS
# =====================================

option( HACK_CPU_EMULATOR_GUI_ENABLE_CLANGTIDY "Enable clang-tidy"        OFF )
option( HACK_CPU_EMULATOR_GUI_ENABLE_CPPCHECK  "Enable cppcheck"          OFF )
option( HACK_CPU_EMULATOR_GUI_ENABLE_IWYU      "Enable iwyu"              ON  )
option( HACK_CPU_EMULATOR_GUI_ENABLE_LWYU      "Enable link whay you use" ON  )

include( StaticAnalyzers )
AddLWYU( Hack_CPU_Emulator_GUI )
add_static_analyzers( Hack_CPU_Emulator_GUI 
   HACK_CPU_EMULATOR_GUI_ENABLE_CLANGTIDY
   HACK_CPU_EMULATOR_GUI_ENABLE_CPPCHECK
   HACK_CPU_EMULATOR_GUI_ENABLE_IWYU
   HACK_CPU_EMULATOR_GUI_ENABLE_LWYU
)


# =====================================
# 	TEST SUITE
# =====================================