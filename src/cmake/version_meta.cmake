if(WIN32)
    configure_file(${VERSION_RC_IN} ${CMAKE_BINARY_DIR}/${TARGET}/version.rc @ONLY)
    set(SOURCES ${SOURCES} ${CMAKE_BINARY_DIR}/${TARGET}/version.rc)
endif()
