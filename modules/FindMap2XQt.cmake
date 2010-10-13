# FindMap2XQt - Map2X Qt handling module for CMake
#
# This module depends on Map2X Core and additionaly defines:
#
# MAP2X_QT_FOUND            - True if Map2X Qt was found
#
# MAP2X_QT_INCLUDE_DIR      - Include dir for Map2X Core
#
# MAP2X_QT_INCLUDE_INSTALL_DIR      - Include installation directory for Qt headers
# MAP2X_PLUGINS_MAPVIEW_INSTALL_DIR - Map view plugins installation directory
# MAP2X_TOOLS_INSTALL_DIR           - Tool plugins installation directory
#

find_package(Map2XCore REQUIRED)

if (MAP2X_QT_INCLUDE_DIR)

    # Already in cache
    set(MAP2X_QT_FOUND TRUE)

else()

    # Paths
    find_path(MAP2X_QT_INCLUDE_DIR
        NAMES AbstractmapView.h AbstractTool.h
        PATH_SUFFIXES Map2X/Qt
    )

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args("Map2X Qt" DEFAULT_MSG
        MAP2X_QT_INCLUDE_DIR
    )

endif()

# Installation dirs
set_parent_scope(MAP2X_QT_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/Map2X/Qt)
set_parent_scope(MAP2X_PLUGINS_MAPVIEW_INSTALL_DIR ${MAP2X_PLUGINS_INSTALL_DIR}/mapViews)
set_parent_scope(MAP2X_PLUGINS_TOOL_INSTALL_DIR ${MAP2X_PLUGINS_INSTALL_DIR}/tools)
