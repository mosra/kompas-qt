# FindKompasQt - Kompas Qt handling module for CMake
#
# This module depends on Kompas Core and additionaly defines:
#
# KOMPASQT_FOUND             - True if Kompas Qt was found
#
# KOMPAS_QT_INCLUDE_DIR      - Include dir for Kompas Core
#
# KOMPAS_QT_INCLUDE_INSTALL_DIR      - Include installation directory for Qt headers
# KOMPAS_PLUGINS_MAPVIEW_INSTALL_DIR - Map view plugins installation directory
# KOMPAS_PLUGINS_UICOMPONENT_INSTALL_DIR - User interface component plugins installation directory
#

find_package(KompasCore REQUIRED)

if (KOMPAS_QT_INCLUDE_DIR)

    # Already in cache
    set(KOMPASQT_FOUND TRUE)

else()

    # Paths
    find_path(KOMPAS_QT_INCLUDE_DIR
        NAMES AbstractMapView.h AbstractUIComponent.h
        PATH_SUFFIXES Kompas/Qt
    )

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args("KompasQt" DEFAULT_MSG
        KOMPAS_QT_INCLUDE_DIR
    )

endif()

# Installation dirs
set_parent_scope(KOMPAS_QT_INCLUDE_INSTALL_DIR ${KOMPAS_INCLUDE_INSTALL_DIR}/Qt)
set_parent_scope(KOMPAS_QT_TRANSLATION_INSTALL_DIR ${KOMPAS_DATA_INSTALL_DIR}/l10n)
set_parent_scope(KOMPAS_PLUGINS_MAPVIEW_INSTALL_DIR ${KOMPAS_PLUGINS_INSTALL_DIR}/mapViews)
set_parent_scope(KOMPAS_PLUGINS_UICOMPONENT_INSTALL_DIR ${KOMPAS_PLUGINS_INSTALL_DIR}/uiComponents)
