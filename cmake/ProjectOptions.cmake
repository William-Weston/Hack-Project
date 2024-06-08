
macro( project_setup )
      
   option( HACK_PROJECT_ENABLE_USER_LINKER         "Enable custom linker"       ON  )
   option( HACK_PROJECT_ENABLE_SANITIZER_ADDRESS   "Enable Address Sanitizer"   ON  )
   option( HACK_PROJECT_ENABLE_SANITIZER_LEAK      "Enable Leak Sanitizer"      ON  )
   option( HACK_PROJECT_ENABLE_SANITIZER_UNDEFINED "Enable Undefined Sanitizer" ON  )
   option( HACK_PROJECT_ENABLE_SANITIZER_THREAD    "Enable Thread Sanitizer"    OFF )

   include( Cache )
   include( IPO )
   enable_ipo()

   # link against the Hack::project_warnings target to enable warnings
   add_library( project_warnings INTERFACE )

   include( CompilerWarnings )
   set_project_warnings( project_warnings )

   add_library( project_options INTERFACE )
   
   include( Linker )
   configure_linker( project_options )
 
   include( Sanitizers )
   enable_sanitizers( project_options 
      ${HACK_PROJECT_ENABLE_SANITIZER_ADDRESS}
      ${HACK_PROJECT_ENABLE_SANITIZER_LEAK}
      ${HACK_PROJECT_ENABLE_SANITIZER_UNDEFINED}
      ${HACK_PROJECT_ENABLE_SANITIZER_THREAD}
   )
   
endmacro()
