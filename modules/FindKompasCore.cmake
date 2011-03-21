# Find KompasCore - Kompas Core handling module for CMake
#
# This module defines:
#
# KOMPASCORE_FOUND           - True if Kompas Core library is found
#
# KOMPAS_CORE_INCLUDE_DIR    - Include dir for Kompas Core
# KOMPAS_PLUGINS_INCLUDE_DIR - Include dir for Kompas plugins
#
# KOMPAS_CORE_LIBRARIES      - Kompas Core libraries
# KOMPAS_RC_EXECUTABLE       - Kompas resource compiler executable
#
# KOMPAS_BINARY_INSTALL_DIR              - Binary installation directory
# KOMPAS_LIBRARY_INSTALL_DIR             - Library installation directory
# KOMPAS_CMAKE_MODULE_INSTALL_DIR        - Installation dir for CMake modules
# KOMPAS_CORE_INCLUDE_INSTALL_DIR        - Include installation directory for Core headers
# KOMPAS_PLUGINS_INCLUDE_INSTALL_DIR     - Include installation directory for plugin headers
# KOMPAS_PLUGINS_INSTALL_DIR             - Plugins installation directory
# KOMPAS_PLUGINS_RASTERMODEL_INSTALL_DIR - Tile model plugins installation directory
# KOMPAS_PLUGINS_PROJECTION_INSTALL_DIR  - Projection plugins installation directory
#

if (KOMPAS_CORE_INCLUDE_DIR AND KOMPAS_PLUGINS_INCLUDE_DIR AND KOMPAS_UTILITY_LIBRARY AND KOMPAS_PLUGINMANAGER_LIBRARY AND KOMPAS_CORE_LIBRARY AND KOMPAS_RC_EXECUTABLE)

    # Already in cache
    set(KOMPASCORE_FOUND TRUE)

else()
    # Libraries
    find_library(KOMPAS_UTILITY_LIBRARY KompasUtility)
    find_library(KOMPAS_PLUGINMANAGER_LIBRARY KompasPluginManager)
    find_library(KOMPAS_CORE_LIBRARY KompasCore)

    # RC executable
    find_program(KOMPAS_RC_EXECUTABLE kompas-rc)

    # Paths
    find_path(KOMPAS_CORE_INCLUDE_DIR
        NAMES PluginManager Utility
        PATH_SUFFIXES Kompas/Core
    )
    find_path(KOMPAS_PLUGINS_INCLUDE_DIR
        NAMES MercatorProjection OpenStreetMapRasterModel
        PATH_SUFFIXES Kompas/Plugins
    )

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args("KompasCore" DEFAULT_MSG
        KOMPAS_CORE_INCLUDE_DIR
        KOMPAS_PLUGINS_INCLUDE_DIR
        KOMPAS_UTILITY_LIBRARY
        KOMPAS_PLUGINMANAGER_LIBRARY
        KOMPAS_CORE_LIBRARY
        KOMPAS_RC_EXECUTABLE
    )

endif()

if(KOMPASCORE_FOUND)
    include(KompasMacros)

    # Installation dirs
    if(WIN32)
        set_parent_scope(KOMPAS_BINARY_INSTALL_DIR .)
        set_parent_scope(KOMPAS_LIBRARY_INSTALL_DIR .)
        set_parent_scope(KOMPAS_CMAKE_MODULE_INSTALL_DIR Modules)
        set_parent_scope(KOMPAS_INCLUDE_INSTALL_DIR include/Kompas)
        set_parent_scope(KOMPAS_PLUGINS_INSTALL_DIR /plugins)
        set_parent_scope(KOMPAS_DATA_INSTALL_DIR )
    else()
        include(KompasLibSuffix)
        set_parent_scope(KOMPAS_BINARY_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/bin)
        set_parent_scope(KOMPAS_LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
        set_parent_scope(KOMPAS_PLUGINS_INSTALL_DIR ${KOMPAS_LIBRARY_INSTALL_DIR}/kompas)
        set_parent_scope(KOMPAS_CMAKE_MODULE_INSTALL_DIR ${CMAKE_ROOT}/Modules)
        set_parent_scope(KOMPAS_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/Kompas)
        set_parent_scope(KOMPAS_DATA_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/share/kompas)
    endif()

    set_parent_scope(KOMPAS_CORE_LIBRARIES ${KOMPAS_UTILITY_LIBRARY} ${KOMPAS_PLUGINMANAGER_LIBRARY} ${KOMPAS_CORE_LIBRARY})

    set_parent_scope(KOMPAS_CORE_INCLUDE_INSTALL_DIR ${KOMPAS_INCLUDE_INSTALL_DIR}/Core)
    set_parent_scope(KOMPAS_PLUGINS_INCLUDE_INSTALL_DIR ${KOMPAS_INCLUDE_INSTALL_DIR}/Plugins)
    set_parent_scope(KOMPAS_PLUGINS_CELESTIALBODY_INSTALL_DIR ${KOMPAS_PLUGINS_INSTALL_DIR}/celestialBodies)
    set_parent_scope(KOMPAS_PLUGINS_PROJECTION_INSTALL_DIR ${KOMPAS_PLUGINS_INSTALL_DIR}/projections)
    set_parent_scope(KOMPAS_PLUGINS_RASTERMODEL_INSTALL_DIR ${KOMPAS_PLUGINS_INSTALL_DIR}/rasterModels)
endif()
