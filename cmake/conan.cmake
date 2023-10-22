include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

conan_basic_setup()
if(WIN32)
    conan_set_vs_runtime()
endif()
conan_check_compiler()

include_directories(${CONAN_INCLUDE_DIRS})
link_directories(${CONAN_LIBS})
