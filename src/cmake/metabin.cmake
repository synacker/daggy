macro(ADD_BINARY_META)
    set(options LIB)
    cmake_parse_arguments(BINARY_META
                          "${options}"
                          ""
                          ""
                          ${ARGN}
    )
    if (NOT ${CMAKE_EXPORTS_PATH})
        set(CMAKE_EXPORTS_PATH ${CMAKE_BINARY_DIR}/exports)
    endif()

    if(WIN32)
        if (NOT BINARY_META_LIB)
            set(FILETYPE "VFT_APP")
        elseif(BUILD_SHARED_LIBS)
            set(FILETYPE "VFT_DLL")
        else()
            set(FILETYPE "VFT_STATIC_LIB")
        endif()

        configure_file(${PROJECT_SOURCE_DIR}/cmake/version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/version.rc @ONLY)
        set(SOURCES ${SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
    endif()
endmacro()
