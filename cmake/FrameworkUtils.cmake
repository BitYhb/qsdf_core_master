macro(check_git_branch_name)
    find_package(Git QUIET)
    if(GIT_FOUND)
        message(STATUS "Git found: ${GIT_EXECUTABLE}")
        execute_process(COMMAND ${GIT_EXECUTABLE} branch --show-current
            OUTPUT_VARIABLE PROJECT_GIT_BRANCH_NAME
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    endif()
endmacro()