function(SCCopyRuntimeDependenciesToTarget destination_target source_target)
    if (NOT WIN32)
        return()
    endif()

    add_custom_command(TARGET ${destination_target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:${source_target}>
            $<TARGET_FILE_DIR:${destination_target}>
        VERBATIM
    )
endfunction()
