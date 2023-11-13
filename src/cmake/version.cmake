include (CMakeParseArguments)

macro(SET_GIT_VERSION)
    cmake_parse_arguments(GIT_VERSION
                          ""
                          ""
                          ""
                          ${ARGN}
    )
    if(NOT DEFINED VERSION)
        find_package(Git REQUIRED)
        if (NOT GIT_FOUND)
            message(FATAL_ERROR "Git not found")
        endif()
        execute_process(COMMAND
                "${GIT_EXECUTABLE}"
                describe
                --match v[0-9]*
                --abbrev=0
                --tags
                WORKING_DIRECTORY
                "${CMAKE_CURRENT_SOURCE_DIR}"
                OUTPUT_VARIABLE
                VERSION_TAG
                OUTPUT_STRIP_TRAILING_WHITESPACE)
        execute_process(COMMAND
                "${GIT_EXECUTABLE}"
                rev-list
                ${VERSION_TAG}..
                --count
                WORKING_DIRECTORY
                "${CMAKE_CURRENT_SOURCE_DIR}"
                OUTPUT_VARIABLE
                VERSION_BUILD
                OUTPUT_STRIP_TRAILING_WHITESPACE)
        execute_process(COMMAND
                "${GIT_EXECUTABLE}"
                branch
                --show-current
                WORKING_DIRECTORY
                "${CMAKE_CURRENT_SOURCE_DIR}"
                OUTPUT_VARIABLE
                VERSION_BRANCH
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        execute_process(COMMAND
                "${GIT_EXECUTABLE}"
                config
                --get
                init.defaultBranch
                WORKING_DIRECTORY
                "${CMAKE_CURRENT_SOURCE_DIR}"
                OUTPUT_VARIABLE
                GIT_DEFAULT_BRANCH
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        if (NOT GIT_DEFAULT_BRANCH)
            set(GIT_DEFAULT_BRANCH master)
        endif()

        string(REGEX REPLACE "^v" "" VERSION ${VERSION_TAG})
        set(VERSION ${VERSION}.${VERSION_BUILD})
        set(VERSION_FULL ${VERSION}-${VERSION_BRANCH})
        if (VERSION_BRANCH STREQUAL ${VERSION_BRANCH} OR VERSION_BRANCH MATCHES "release/.*")
            set(VERSION_STANDARD ${VERSION})
        else()
            set(VERSION_STANDARD ${VERSION_FULL})
        endif()

        execute_process(COMMAND
                "${GIT_EXECUTABLE}"
                rev-parse
                HEAD
                WORKING_DIRECTORY
                "${CMAKE_CURRENT_SOURCE_DIR}"
                OUTPUT_VARIABLE
                VERSION_COMMIT
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    endif()
    message(STATUS
        "Version was set:\n"
        "Tag: ${VERSION_TAG}\n"
        "Version: ${VERSION}\n"
        "Full: ${VERSION_FULL}\n"
        "Branch: ${VERSION_BRANCH}\n"
        "Build: ${VERSION_BUILD}\n"
        "Standard: ${VERSION_STANDARD}\n"
        "Commit: ${VERSION_COMMIT}"
    )
endmacro()
