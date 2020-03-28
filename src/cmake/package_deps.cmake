if(WIN32)
    install(FILES
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Qt5Network.dll
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Qt5Core.dll
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libssh2.dll
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/yaml-cpp.dll
        DESTINATION bin
        COMPONENT deps
    )
else()
    install(DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/daggy_deps/
            DESTINATION lib/daggy_deps
            COMPONENT deps
            FILES_MATCHING 
            PATTERN libQt5Network.so*
            PATTERN libQt5Core.so*
            PATTERN libssh2.so*
            PATTERN libyaml-cpp.so*
    )
endif()