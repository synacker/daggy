macro(ADD_BINARY_META)
    set(options LIB)
    cmake_parse_arguments(BINARY_META
                          "${options}"
                          ""
                          ""
                          ${ARGN}
    )
    if(WIN32)
        if (NOT BINARY_META_LIB)
            set(FILETYPE "VFT_APP")
        elseif(BUILD_SHARED_LIBS)
            set(FILETYPE "VFT_DLL")
        else()
            set(FILETYPE "VFT_STATIC_LIB")
        endif()

        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/version.rc.in ${CMAKE_BINARY_DIR}/${TARGET}/version.rc @ONLY)
        set(SOURCES ${SOURCES} ${CMAKE_BINARY_DIR}/${TARGET}/version.rc)
    endif()
endmacro()
