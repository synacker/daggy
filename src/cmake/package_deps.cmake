if(UNIX AND NOT APPLE)
    install(DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/daggy/
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/daggy
            FILES_MATCHING
            PATTERN libQt6Network.so*
            PATTERN libQt6Core.so*
            PATTERN libssh2.so*
            PATTERN libyaml-cpp.so*
            PATTERN libcrypto*.so*
    )
elseif(APPLE)
    install(DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/daggy/
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/daggy
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
    install(FILES
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Qt6Network.dll
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Qt6Core.dll
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libssh2.dll
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/yaml-cpp.dll
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libcrypto-1_1-x64.dll
            ${CMAKE_CURRENT_LIST_DIR}/daggyenv.bat
            DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
