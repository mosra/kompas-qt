cmake_minimum_required(VERSION 2.6)

project(Map2XQt)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${Map2XQt_SOURCE_DIR}/modules/")

include(Map2XMacros)

set_parent_scope(MAP2X_QT_INCLUDE_DIR       "${CMAKE_CURRENT_SOURCE_DIR}/src")

include(FindMap2XQt)

add_subdirectory(modules)
add_subdirectory(src)

option(BUILD_TESTS "Build unit tests (requires Qt4)." OFF)

if(BUILD_TESTS)
    find_package(Qt4)

    if(NOT QT4_FOUND)
        message(WARNING "Qt4 is required for building unit tests. No tests will be build.")
        set(BUILD_TESTS OFF)
    else()
        enable_testing()
    endif()

endif()

install(DIRECTORY src/ DESTINATION ${MAP2X_QT_INCLUDE_INSTALL_DIR}
                 FILES_MATCHING PATTERN "*.h"
                 PATTERN "*/Test" EXCLUDE
                 PATTERN src/Plugins EXCLUDE)
install(DIRECTORY src/Plugins/ DESTINATION ${MAP2X_PLUGINS_INCLUDE_INSTALL_DIR}
                FILES_MATCHING PATTERN "*.h"
                PATTERN "*/Test" EXCLUDE)
