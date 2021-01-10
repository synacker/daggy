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
    set(VC_REDIST ${CMAKE_BINARY_DIR}/vc_redist-x64.exe)
    if(NOT EXISTS ${VC_REDIST})
        execute_process(COMMAND powershell  wget -UseBasicParsing -OutFile ${VC_REDIST} 'https://aka.ms/vs/16/release/vc_redist.x64.exe')
    endif()
    install(FILES ${VC_REDIST}
            DESTINATION vcredist
            COMPONENT deps)
    install(FILES
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Qt6Network.dll
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Qt6Core.dll
            ${CMAKE_CURRENT_LIST_DIR}/daggyenv.bat
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT deps)
endif()
