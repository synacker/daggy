macro(SET_RPATH)
    set(options LIB)
    cmake_parse_arguments(RPATH_ENV
                          "${options}"
                          ""
                          ""
                          ${ARGN}
    )
    if(CONAN_BUILD)
        if (NOT RPATH_ENV_LIB)
            if(APPLE)
                set(CMAKE_INSTALL_RPATH "@executable_path/../${CMAKE_INSTALL_LIBDIR}")
            elseif(UNIX)
                set(CMAKE_INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
            endif()
        elseif(BUILD_SHARED_LIBS)
            if(APPLE)
                set(CMAKE_INSTALL_RPATH "@loader_path")
            elseif(UNIX)
                set(CMAKE_INSTALL_RPATH "$ORIGIN")
            endif()
        endif()
        if (PACKAGE_DEPS AND CMAKE_INSTALL_RPATH)
            set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH};${CMAKE_INSTALL_RPATH}/${PROJECT_NAME}")
        endif()
    endif()
endmacro()

