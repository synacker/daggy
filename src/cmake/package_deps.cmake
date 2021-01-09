if(UNIX AND NOT APPLE)
    install(DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/daggy/
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/daggy
            COMPONENT deps
            FILES_MATCHING
            PATTERN libQt6Network.so*
            PATTERN libQt6Core.so*
    )
elseif(APPLE)
    install(DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/daggy/
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/daggy
            COMPONENT deps
            FILES_MATCHING
            PATTERN libQt6Network*dylib
            PATTERN libQt6Core*dylib
    )
else()
    install(FILES
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Qt6Network.dll
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Qt6Core.dll
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT deps)
endif()
