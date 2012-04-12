cmake_minimum_required(VERSION 2.6)

project(KompasQt)

# Version
set(KOMPAS_QT_VERSION 0.1.60)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${KompasQt_SOURCE_DIR}/modules/")

find_package(KompasCore REQUIRED)
find_package(Qt4 REQUIRED)

option(BUILD_TESTS "Build unit tests." OFF)

if(BUILD_TESTS)
    enable_testing()
endif()

set_parent_scope(KOMPAS_QT_INCLUDE_DIR      "${CMAKE_CURRENT_SOURCE_DIR}/src")
set_parent_scope(KOMPAS_QT_LIBRARY          KompasQt)

include(FindKompasQt)

# Library version
set(KOMPAS_QT_LIBRARY_VERSION 0.1.1)
set(KOMPAS_QT_LIBRARY_SOVERSION 0)

add_subdirectory(l10n)
add_subdirectory(modules)
add_subdirectory(graphics)
add_subdirectory(package)
add_subdirectory(src)

if(NOT WIN32)
    install(DIRECTORY src/ DESTINATION ${KOMPAS_QT_INCLUDE_INSTALL_DIR}
                 FILES_MATCHING PATTERN "*.h"
                 PATTERN "*/Test" EXCLUDE
                 PATTERN src/Plugins EXCLUDE)
    install(DIRECTORY src/Plugins/ DESTINATION ${KOMPAS_PLUGINS_INCLUDE_INSTALL_DIR}
                FILES_MATCHING PATTERN "*.h"
                PATTERN "*/Test" EXCLUDE)
endif()
