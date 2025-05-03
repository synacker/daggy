set(TARGET daggy)

option(PCAPNG_SUPPORT "pcap to pcapng convertor" OFF)

find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(${TARGET})
include(rpath_bin)
set_target_properties(${TARGET} PROPERTIES AUTOMOC ON)

target_sources(${TARGET}
    PRIVATE
        main.cpp
        CConsoleDaggy.cpp
)

target_link_libraries(${TARGET} PRIVATE DaggyCore)

if (PCAPNG_SUPPORT)
    add_compile_definitions(PCAPNG_SUPPORT)
    find_package(PcapPlusPlus REQUIRED)
    target_link_libraries(${TARGET} PRIVATE PcapPlusPlus::PcapPlusPlus)
endif()

if (CONAN_BUILD)
    find_package(kainjow_mustache REQUIRED)
    target_link_libraries(${TARGET} PRIVATE kainjow_mustache::kainjow_mustache)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/simple_sources.yaml ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/simple_sources.json ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COPYONLY)

add_definitions(-DAPP_ICON="${CMAKE_CURRENT_LIST_DIR}/../icons/daggy.ico")


target_link_libraries(${TARGET} PRIVATE ${LINK_LIBRARIES})
target_precompile_headers(${TARGET} PRIVATE Precompiled.hpp)

install(TARGETS ${TARGET} RUNTIME)
include(package_deps)
