add_llvm_library(InliningPlugin  MODULE inline_plugin.cpp
                 PLUGIN_TOOL clang)

if(WIN32 OR CYGWIN)
    set(LLVM_LINK_COMPONENTS Support)
    clang_target_link_libraries(InliningPlugin PRIVATE
        clangASTMatchers
        clangBasic
        clangFronten
        clangTooling
    )
endif()

set_target_properties(InliningPlugin
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
