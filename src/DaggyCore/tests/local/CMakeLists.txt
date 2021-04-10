set(TARGET DaggyCoreLocalTests)

find_package(Qt6Test REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


set(SOURCES
    DaggyCoreLocalTests.cpp
    main.cpp
)

if(APPLE)
    set(CMAKE_INSTALL_RPATH "@executable_path/../${CMAKE_INSTALL_LIBDIR}/;@executable_path/../${CMAKE_INSTALL_LIBDIR}/daggy")
elseif(UNIX)
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}/daggy;$ORIGIN/../${CMAKE_INSTALL_LIBDIR};")
endif()

enable_testing(true)
add_executable(${TARGET} ${SOURCES})
add_test(NAME ${TARGET} COMMAND ${TARGET})

target_link_libraries(${TARGET} PRIVATE Qt6::Test)

include(precompiled)
