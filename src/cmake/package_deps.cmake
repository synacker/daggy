if(WIN32)
    install(FILES
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Qt6Network.dll
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Qt6Core.dll
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT deps
    )
else()
    install(DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/daggy/
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/daggy
            COMPONENT deps
            FILES_MATCHING 
            PATTERN libQt6Network.so*
            PATTERN libQt6Core.so*
    )
endif()
