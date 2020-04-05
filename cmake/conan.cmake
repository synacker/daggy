include(conan_cmake_wrapper)

if(NOT EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
        conan_cmake_run(CONANFILE conanfile.py
                        BUILD missing
        )
endif()
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(NO_OUTPUT_DIRS)
conan_define_targets()
conan_set_std()
conan_set_libcxx()
if(WIN32)
    conan_set_vs_runtime()
endif()
conan_check_compiler()
include_directories(${CONAN_INCLUDE_DIRS})
link_directories(${CONAN_LIBS})
