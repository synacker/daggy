find_package(Git)
if (NOT DEFINED VERSION)
    if (GIT_FOUND)
        execute_process(COMMAND
                "${GIT_EXECUTABLE}"
                describe
                --match [0-9]*.[0-9]*.[0-9]*
                --abbrev=0
                --tags
                WORKING_DIRECTORY
                "${CMAKE_CURRENT_SOURCE_DIR}"
                OUTPUT_VARIABLE
                VERSION
                OUTPUT_STRIP_TRAILING_WHITESPACE)
        execute_process(COMMAND
                "${GIT_EXECUTABLE}"
                rev-list
                ${VERSION}..
                --count
                WORKING_DIRECTORY
                "${CMAKE_CURRENT_SOURCE_DIR}"
                OUTPUT_VARIABLE
                BUILD_NUMBER
                OUTPUT_STRIP_TRAILING_WHITESPACE)
    endif(GIT_FOUND)

    string(REPLACE "." ";" VERSION_LIST ${VERSION})
    list(GET VERSION_LIST 0 PROJECT_VERSION_MAJOR)
    list(GET VERSION_LIST 1 PROJECT_VERSION_MINOR)
    list(GET VERSION_LIST 2 PROJECT_VERSION_PATCH)

    set(VERSION ${VERSION}.${BUILD_NUMBER})
endif()

if (NOT DEFINED BUILD_NUMBER)
    set(BUILD_NUMBER 0)
endif()


set(PROJECT_VERSION ${VERSION})

add_definitions(-DVERSION_MAJOR=${PROJECT_VERSION_MAJOR})
add_definitions(-DVERSION_MINOR=${PROJECT_VERSION_MINOR})
add_definitions(-DVERSION_PATCH=${PROJECT_VERSION_PATCH})
add_definitions(-DVERSION_BUILD=${BUILD_NUMBER})
add_definitions(-DVERSION=${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})

add_definitions(-DVERSION_STR="${VERSION}")

if (WIN32)
    set(VERSION_RC_IN ${CMAKE_CURRENT_LIST_DIR}/version.rc.in)
endif()
