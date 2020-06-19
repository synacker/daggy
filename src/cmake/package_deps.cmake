if(WIN32)
    install(FILES
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Qt5Network.dll
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Qt5Core.dll
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT deps
    )
else()
    install(DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/daggy/
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/daggy
            COMPONENT deps
            FILES_MATCHING 
            PATTERN libQt5Network.so*
            PATTERN libQt5Core.so*
    )
endif()
