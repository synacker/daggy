if (PACKAGE_DEPS)
    install(TARGETS ${TARGET}
            RUNTIME_DEPENDENCY_SET ${TARGET}_deps
    )

    install(RUNTIME_DEPENDENCY_SET ${TARGET}_deps
            PRE_EXCLUDE_REGEXES
                [=[api-ms-]=]
                [=[ext-ms-]=]
                [[kernel32\.dll]]
                [[libc\.so\..*]] [[libgcc_s\.so\..*]] [[libm\.so\..*]] [[libstdc\+\+\.so\..*]]
            POST_EXCLUDE_REGEXES
                [=[.*system32\/.*\.dll]=]
                [=[^\/(lib|usr\/lib|usr\/local\/lib\/lib64)]=]
            DIRECTORIES ${CONAN_RUNTIME_LIB_DIRS} ${CONAN_LIB_DIRS}
    )
endif()
