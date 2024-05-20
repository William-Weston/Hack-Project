if( PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR )
   message( FATAL_ERROR
        "\nError:"
        "\tIn-source builds are not permitted.\n"
        "\tProvide build path.  Usage:\n"
        "\t\tcmake -B <build-path>\n"
        "\n"
        "\tTo remove files accidentally created execute:\n"
        "\t\trm -rf CMakeFiles CMakeCache.txt\n\n"
)
endif()