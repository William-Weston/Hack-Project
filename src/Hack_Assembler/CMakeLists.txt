cmake_minimum_required( VERSION 3.26 )

# =====================================
# Define Targets
# =====================================

add_library( Hack_Assembler )
add_library( Hack::Assembler ALIAS Hack_Assembler )

target_sources( Hack_Assembler 
   PRIVATE 
      include/Hack/Assembler.h
      include/Hack/Code_Line.h 
      include/Hack/Symbol_Table.h
      src/Assembler.cpp
      src/Code_Generator.h 
      src/Code_Generator.cpp
      src/Symbol_Table.cpp
)

set( HACK_ASSEMBLER_PUBLIC_HEADERS
   "include/Hack/Assembler.h"
   "include/Hack/Code_Line.h "
   "include/Hack/Symbol_Table.h"
)

set_target_properties( Hack_Assembler 
   PROPERTIES 
      PUBLIC_HEADER "${HACK_ASSEMBLER_PUBLIC_HEADERS}"
)

target_include_directories( Hack_Assembler
   PUBLIC 
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
      "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
   PRIVATE
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Hack>"
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
)


target_link_libraries( Hack_Assembler 
   PUBLIC
      tl::expected
   PRIVATE
      Hack::project_warnings
      Hack::Utilities
)

include( Coverage )
CleanCoverage( Hack_Assembler )
EnableCoverage( Hack_Assembler )

include( StaticAnalyzers )
AddCppCheck( Hack_Assembler )
AddClangTidy( Hack_Assembler )


# =====================================
# 	TEST SUITE
# =====================================

add_executable( Hack_Assembler_Tests )

target_sources( Hack_Assembler_Tests 
   PRIVATE
      src/Assembler.t.cpp
)


target_link_libraries( Hack_Assembler_Tests 
   PRIVATE 
      Catch2::Catch2 
      Catch2::Catch2WithMain
      Hack::project_warnings
      Hack::Assembler
      Hack::Utilities
)

enable_testing()

if( PROJECT_IS_TOP_LEVEL )
   include( CTest )
endif()

include( Coverage )
AddCoverage( Hack_Assembler_Tests )

include( Catch )
catch_discover_tests( Hack_Assembler_Tests )