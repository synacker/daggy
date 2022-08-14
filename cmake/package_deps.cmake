set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
if(UNIX AND NOT APPLE)
    install(DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/daggy/
            DESTINATION .
            FILES_MATCHING
            PATTERN libQt6Network.so*
            PATTERN libQt6Core.so*
            PATTERN libssh2.so*
            PATTERN libyaml-cpp.so*
            PATTERN libcrypto*.so*
    )
elseif(APPLE)
    install(DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/daggy/
            DESTINATION .
            FILES_MATCHING
            PATTERN libQt6Network*dylib
            PATTERN libQt6Core*dylib
            PATTERN libssh2*dylib
            PATTERN libyaml-cpp*dylib
            PATTERN libcrypto*dylib
    )
else()
    set(VC_REDIST ${CMAKE_BINARY_DIR}/vc_redist-x64.exe)
    if(NOT EXISTS ${VC_REDIST})
        execute_process(COMMAND powershell wget -UseBasicParsing -OutFile ${VC_REDIST} 'https://aka.ms/vs/16/release/vc_redist.x64.exe')
    endif()
    install(FILES ${VC_REDIST}
            DESTINATION vcredist)
    install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            DESTINATION .
            FILES_MATCHING
            PATTERN Qt6Network.dll
            PATTERN Qt6Core.dll
            PATTERN libssh2.dll
            PATTERN yaml-cpp.dll
            PATTERN libcrypto-1_1-x64.dll)
    install(FILES
            ${CMAKE_CURRENT_LIST_DIR}/daggyenv.bat
            DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
