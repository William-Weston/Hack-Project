# =====================================
# Preamble
# =====================================

cmake_minimum_required( VERSION 3.29 )

project( Hack_Computer_Project 
		VERSION 			0.0.1.0
		DESCRIPTION		"Hack Computer Project"
		HOMEPAGE_URL 	"www.MyProj.com"
		LANGUAGES		CXX
)

list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )

include( PreventInSourceBuilds )

 
# =====================================
# Project Wide Setup
# =====================================

set( CMAKE_CXX_STANDARD 23 )
set( CMAKE_CXX_STANDARD_REQUIRED YES )
set( CMAKE_CXX_EXTENSIONS NO )

set( CMAKE_THREAD_PREFER_PTHREAD TRUE )
set( THREADS_PREFER_PTHREAD_FLAG TRUE )

# Generate compile_commands.json, enhance error reporting & compiler messages, and set default build
include( StandardProjectSettings )

# link to Hack::hack_config to add configure file to target, then add #include "Hack/hack_config.hpp"
configure_file( Hack_config.hpp.in "${CMAKE_CURRENT_BINARY_DIR}/include/Hack/hack_config.hpp" @ONLY )
add_library( hack_config INTERFACE )
add_library( Hack::hack_config ALIAS hack_config )
target_include_directories( hack_config
   INTERFACE
      ${CMAKE_CURRENT_BINARY_DIR}/include
)

include( ProjectOptions )
project_setup()

add_library( Hack::project_warnings ALIAS project_warnings )
add_library( Hack::project_options ALIAS project_options )


if( PROJECT_IS_TOP_LEVEL )
   include( CTest )
endif()

enable_testing()

# =====================================
# Externally Provided Content
# =====================================

include( dependencies.cmake )
add_subdirectory( external )


# =====================================
# Main Targets Built By This Project
# =====================================

add_subdirectory( src )

# Not typically needed if there is a parent project
if( PROJECT_IS_TOP_LEVEL )
	add_subdirectory( tests )
#	add_subdirectory( packaging )
endif() 



