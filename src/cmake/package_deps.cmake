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
    install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/daggy_deps/
            DESTINATION lib
            COMPONENT deps
            FILES_MATCHING 
            PATTERN ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/daggy_deps/libQt5Network.so*
            PATTERN ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/daggy_deps/libQt5Core.so*
            PATTERN ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/daggy_deps/libssh2.so*
            PATTERN ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/daggy_deps/libyaml-cpp.so*
    )
endif()
