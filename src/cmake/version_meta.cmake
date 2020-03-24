if(WIN32)
    configure_file(${VERSION_RC_IN} ${CMAKE_CURRENT_LIST_DIR}/version.rc @ONLY)
    set(SOURCES ${SOURCES} version.rc)
endif()