string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
configure_file(${PROJECT_SOURCE_DIR}/cmake/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/exports/${TARGET}/version.h @ONLY)
configure_file(${PROJECT_SOURCE_DIR}/cmake/version.c.in ${CMAKE_CURRENT_BINARY_DIR}/version.c @ONLY)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/exports/${TARGET}/)

set(SOURCES ${SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/version.c)
