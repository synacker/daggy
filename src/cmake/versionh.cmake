string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
configure_file(${PROJECT_SOURCE_DIR}/cmake/version.h.in ${CMAKE_BINARY_DIR}/exports/${TARGET}/version.h @ONLY)

include_directories(${CMAKE_BINARY_DIR}/exports/${TARGET}/)
