include(conan_cmake_wrapper)

if(NOT EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
        conan_cmake_run(CONANFILE conanfile.py
                        OPTIONS shared=True
                        GENERATORS cmake_find_package cmake_paths
                        BUILD missing
        )
endif()
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

conan_basic_setup(TARGETS NO_OUTPUT_DIRS KEEP_RPATHS)
if(WIN32)
    conan_set_vs_runtime()
endif()
conan_check_compiler()

include_directories(${CONAN_INCLUDE_DIRS})
link_directories(${CONAN_LIBS})
