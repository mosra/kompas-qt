# FindKompasQt - Kompas Qt handling module for CMake
#
# This module depends on Kompas Core and additionaly defines:
#
# KOMPAS_QT_FOUND            - True if Kompas Qt was found
#
# KOMPAS_QT_INCLUDE_DIR      - Include dir for Kompas Core
#
# KOMPAS_QT_INCLUDE_INSTALL_DIR      - Include installation directory for Qt headers
# KOMPAS_PLUGINS_MAPVIEW_INSTALL_DIR - Map view plugins installation directory
# KOMPAS_TOOLS_INSTALL_DIR           - Tool plugins installation directory
#

find_package(KompasCore REQUIRED)

if (KOMPAS_QT_INCLUDE_DIR)

    # Already in cache
    set(KOMPAS_QT_FOUND TRUE)

else()

    # Paths
    find_path(KOMPAS_QT_INCLUDE_DIR
        NAMES AbstractmapView.h AbstractTool.h
        PATH_SUFFIXES Kompas/Qt
    )

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args("Kompas Qt" DEFAULT_MSG
        KOMPAS_QT_INCLUDE_DIR
    )

endif()

# Installation dirs
set_parent_scope(KOMPAS_QT_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/Kompas/Qt)
set_parent_scope(KOMPAS_QT_TRANSLATION_INSTALL_DIR ${KOMPAS_DATA_INSTALL_DIR}/l10n)
set_parent_scope(KOMPAS_PLUGINS_MAPVIEW_INSTALL_DIR ${KOMPAS_PLUGINS_INSTALL_DIR}/mapViews)
set_parent_scope(KOMPAS_PLUGINS_TOOL_INSTALL_DIR ${KOMPAS_PLUGINS_INSTALL_DIR}/tools)
