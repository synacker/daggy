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
    install(FILES
        PATTERN
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libQt5Network.so*
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libQt5Core.so*
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libssh2.so*
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libyaml-cpp.so*
        DESTINATION lib
        COMPONENT deps
    )
endif()
