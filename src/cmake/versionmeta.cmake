if(WIN32)
    configure_file(${CMAKE_PROJECT_MODULES}/version.rc.in ${CMAKE_BINARY_DIR}/${TARGET}/version.rc @ONLY)
    target_sources(${TARGET} PRIVATE ${CMAKE_BINARY_DIR}/${TARGET}/version.rc)
endif()