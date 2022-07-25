if(CONAN_BUILD)
    if(APPLE)
        set_target_properties(${TARGET} PROPERTIES INSTALL_NAME_DIR "@executable_path/../${CMAKE_INSTALL_LIBDIR};@executable_path/../${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}")
    elseif(UNIX)
        set_target_properties(${TARGET} PROPERTIES INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}:$ORIGIN/../${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}")
    endif()
endif()
