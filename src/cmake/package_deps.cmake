set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
if(UNIX AND NOT APPLE)
        install(DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/daggy/
                DESTINATION ${CMAKE_INSTALL_LIBDIR}/daggy/
                FILES_MATCHING
                PATTERN libQt6Network.so*
                PATTERN libQt6Core.so*
                PATTERN libssh2.so*
                PATTERN libyaml-cpp.so*
                PATTERN libcrypto*.so*
        )
elseif(APPLE)
        install(DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/daggy/
                DESTINATION ${CMAKE_INSTALL_LIBDIR}/daggy/
                FILES_MATCHING
                PATTERN libQt6Network*dylib
                PATTERN libQt6Core*dylib
                PATTERN libssh2*dylib
                PATTERN libyaml-cpp*dylib
                PATTERN libcrypto*dylib
        )
else()
        install(TARGETS DaggyCore
                DESTINATION ${CMAKE_INSTALL_BINDIR}
                RUNTIME_DEPENDENCY_SET daggy_core_deps
        )

        install(RUNTIME_DEPENDENCY_SET daggy_core_deps
                PRE_EXCLUDE_REGEXES
                        [[api-ms-win-.*]]
                        [[ext-ms-.*]]
                        [[kernel32\.dll]]
                POST_EXCLUDE_REGEXES
                        [[.*(\\|/)system32(\\|/).*\.dll]]
                DIRECTORIES ${CONAN_RUNTIME_LIB_DIRS} ${CONAN_LIB_DIRS}
        )

        if (CONSOLE)
                install(TARGETS daggy
                        DESTINATION ${CMAKE_INSTALL_BINDIR}
                        RUNTIME_DEPENDENCY_SET daggy_deps
                )

                install(RUNTIME_DEPENDENCY_SET daggy_deps
                        PRE_EXCLUDE_REGEXES
                                [[api-ms-win-.*]]
                                [[ext-ms-.*]]
                                [[kernel32\.dll]]
                        POST_EXCLUDE_REGEXES
                                [[.*(\\|/)system32(\\|/).*\.dll]]
                        DIRECTORIES ${CONAN_RUNTIME_LIB_DIRS} ${CONAN_LIB_DIRS}
                )

                install(FILES
                        ${CMAKE_CURRENT_LIST_DIR}/daggyenv.bat
                        DESTINATION ${CMAKE_INSTALL_BINDIR})
        endif()
endif()
