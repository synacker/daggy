find_package(Git)
if (GIT_FOUND)
    execute_process(COMMAND
            "${GIT_EXECUTABLE}"
            describe
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

if(WIN32)
    set(VERSION "${VERSION}.${BUILD_NUMBER}")
    set(FULL_VERSION "${VERSION}")
else()
    set(FULL_VERSION "${VERSION}.${BUILD_NUMBER}")
endif(WIN32)

set(PROJECT_VERSION ${FULL_VERSION})
string(REPLACE "." ";" VERSION_LIST ${VERSION})
list(GET VERSION_LIST 0 PROJECT_VERSION_MAJOR)
list(GET VERSION_LIST 1 PROJECT_VERSION_MINOR)
list(GET VERSION_LIST 2 PROJECT_VERSION_PATCH)

add_definitions(-DVERSION_MAJOR=${PROJECT_VERSION_MAJOR})
add_definitions(-DVERSION_MINOR=${PROJECT_VERSION_MINOR})
add_definitions(-DVERSION_PATCH=${PROJECT_VERSION_PATCH})
add_definitions(-DVERSION_BUILD=${BUILD_NUMBER})
add_definitions(-DVERSION=${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
add_definitions(-DFULL_VERSION=${VERSION}.${BUILD_NUMBER})

add_definitions(-DVERSION_STR="${VERSION}")
add_definitions(-DFULL_VERSION_STR="${FULL_VERSION}")
