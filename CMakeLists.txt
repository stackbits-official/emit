cmake_minimum_required(VERSION 3.21)

set(EMIT_VERSION_MAJOR 0)
set(EMIT_VERSION_MINOR 0)
set(EMIT_VERSION_PATCH 0)

project(emit LANGUAGES CXX VERSION ${EMIT_VERSION_MAJOR}.${EMIT_VERSION_MINOR}.${EMIT_VERSION_PATCH})

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(emit INTERFACE)
add_library(emit::emit ALIAS emit)

target_include_directories(emit
    INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_definitions(emit 
    INTERFACE 
    EMIT_VERSION_MAJOR=${EMIT_VERSION_MAJOR}
    EMIT_VERSION_MINOR=${EMIT_VERSION_MINOR}
    EMIT_VERSION_PATCH=${EMIT_VERSION_PATCH}
)

install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    TARGETS emit ${EMIT_MODULE_TARGETS}
    EXPORT emitTargets
)

install(
    EXPORT emitTargets
    NAMESPACE emit::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/emit
)

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/emitConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/emitConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/emit
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/emitConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/emit
)