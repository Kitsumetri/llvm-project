add_llvm_library(AlwaysInline MODULE AlwaysInline.cpp
                 PLUGIN_TOOL clang)

if(WIN32 OR CYGWIN)
    set(LLVM_LINK_COMPONENTS Support)
    clang_target_link_libraries(AlwaysInline PRIVATE
        clangAST
        clangBasic
        clangFrontend
        clangTooling
    )
endif()

set_target_properties(AlwaysInline
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
