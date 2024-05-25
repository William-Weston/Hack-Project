cmake_minimum_required( VERSION 3.29 )

# =====================================
# Define Targets
# =====================================

add_library( Hack_Computer )
add_library( Hack::Computer ALIAS Hack_Computer )

target_sources( Hack_Computer
   PRIVATE 
      include/Hack/Computer.h
      include/Hack/CPU.h
      include/Hack/Memory.h
      src/ALU.h
      src/Computer.cpp
      src/CPU.cpp
)

set( HACK_COMPUTER_PUBLIC_HEADERS
   "include/Hack/Computer.h"
   "include/Hack/CPU.h"
   "include/Hack/Memory.h"
)

set_target_properties( Hack_Computer 
   PROPERTIES 
      PUBLIC_HEADER "${HACK_COMPUTER_PUBLIC_HEADERS}"
)


target_include_directories( Hack_Computer
   PUBLIC 
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
      "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
   PRIVATE
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Hack>"
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
)

target_link_libraries( Hack_Computer
   PRIVATE 
      Hack::project_warnings 
      Hack::Utilities
)


include( Coverage )
CleanCoverage( Hack_Computer )
EnableCoverage( Hack_Computer )


# =====================================
# 	OPTIONS
# =====================================

option( HACK_COMPUTER_ENABLE_CLANGTIDY "Enable clang-tidy"        OFF )
option( HACK_COMPUTER_ENABLE_CPPCHECK  "Enable cppcheck"          OFF )
option( HACK_COMPUTER_ENABLE_IWYU      "Enable iwyu"              ON  )
option( HACK_COMPUTER_ENABLE_LWYU      "Enable link whay you use" ON  )

include( StaticAnalyzers )

EnableSanitizers( Hack_Computer 
   HACK_COMPUTER_ENABLE_CLANGTIDY
   HACK_COMPUTER_ENABLE_CPPCHECK
   HACK_COMPUTER_ENABLE_IWYU
   HACK_COMPUTER_ENABLE_LWYU
)


# =====================================
# 	TEST SUITE
# =====================================


add_executable( Hack_Computer_Tests )

target_sources( Hack_Computer_Tests 
   PRIVATE
      src/ALU.t.cpp
      src/Computer.t.cpp
      src/CPU.t.cpp
      src/Memory.t.cpp
)

target_link_libraries( Hack_Computer_Tests 
   PRIVATE 
      Catch2::Catch2 
      Catch2::Catch2WithMain
      Hack::project_warnings
      Hack::Computer
      Hack::Utilities
)


include( Coverage )
AddCoverage( Hack_Computer_Tests )

enable_testing()

if( PROJECT_IS_TOP_LEVEL )
   include( CTest )
endif()

include( Catch )
catch_discover_tests( Hack_Computer_Tests )