cmake_minimum_required( VERSION 3.29 )

# =====================================
# Define Targets
# =====================================

add_library( GUI_Core )
add_library( GUI_Core::GUI_Core ALIAS GUI_Core )

target_sources( GUI_Core 
   PUBLIC
      FILE_SET    HEADERS
      BASE_DIRS   include
      FILES       include/GUI_Core/GUI_Core.h
                  include/GUI_Core/GUI_Frame.h
                  include/GUI_Core/SDL_InitError.h

   PRIVATE
      src/GUI_Core.cpp
      src/GUI_Frame.cpp
)

target_include_directories( GUI_Core 
   PRIVATE 
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/GUI_Core>
)

target_link_libraries( GUI_Core
   PUBLIC
      $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
      $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
      imgui::imgui

   PRIVATE
      Hack::project_warnings
      Hack::project_options
)

# Static Analysis Tools
option( HACK_GUI_CORE_ENABLE_CLANGTIDY  "Enable Clang-tidy" ON  )
option( HACK_GUI_CORE_ENABLE_CPPCHECK   "Enable CppCheck"   ON  )

include( StaticAnalyzers )
add_static_analyzers( GUI_Core 
   "OFF"
   "${HACK_GUI_CORE_ENABLE_CPPCHECK}"
   "ON"
   "ON"
)