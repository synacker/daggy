macro(SET_RPATH)
    if(CONAN_BUILD)
        set(options LIB)
        cmake_parse_arguments(BINARY_META
                              "${options}"
                              ""
                              ""
                              ${ARGN}
        )
        if (NOT LIB)
            if(APPLE)
                set(CMAKE_INSTALL_RPATH "@loader_path/../${CMAKE_INSTALL_LIBDIR}/;@loader_path/../${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}")
            elseif(UNIX)
                set(CMAKE_INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME};$ORIGIN/../${CMAKE_INSTALL_LIBDIR};")
            endif()
        elseif(BUILD_SHARED_LIBS)
            if(APPLE)
                set(CMAKE_INSTALL_RPATH "@loader_path/${PROJECT_NAME}")
            elseif(UNIX)
                set(CMAKE_INSTALL_RPATH "$ORIGIN/${PROJECT_NAME};")
            endif()
        endif ()
    endif()
endmacro()
