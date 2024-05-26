include_guard( GLOBAL )

set ( BUILDINFO_TEMPLATE_DIR ${CMAKE_CURRENT_LIST_DIR} )
set ( DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/buildinfo" )

string ( TIMESTAMP TIMESTAMP "%Y-%m-%d %H:%M:%S" )

find_program( GIT_PATH git REQUIRED )

execute_process( 
   COMMAND 
      ${GIT_PATH} log --pretty=format:'%h' -n 1
   OUTPUT_VARIABLE
      COMMIT_SHA
)


configure_file(
   "${BUILDINFO_TEMPLATE_DIR}/buildinfo.h.in"
   "${DESTINATION}/Hack/buildinfo.h" 
   @ONLY
)

add_library( BuildInfo INTERFACE )
set_target_properties( BuildInfo 
   PROPERTIES 
      PUBLIC_HEADER "${DESTINATION}/Hack/buildinfo.h"
)

target_include_directories( BuildInfo INTERFACE ${DESTINATION} )