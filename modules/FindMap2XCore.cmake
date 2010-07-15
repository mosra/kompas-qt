# Find Map2XCore - Map2X Core handling module for CMake
#
# This module defines:
#
# MAP2X_CORE_FOUND          - True if Map2X Core library is found
# MAP2X_CORE_INCLUDE_DIR    - Include dir for Map2X Core
# MAP2X_CORE_LIBRARY        - Map2X Core library
#

if (MAP2X_CORE_INCLUDE_DIR AND MAP2X_CORE_LIBRARY)

    # Already in cache
    set(MAP2X_FOUND TRUE)

else()

    # Libraries
    find_library (MAP2X_CORE_LIBRARY Map2XCore)

    # Paths
    find_path(MAP2X_CORE_INCLUDE_DIR
        NAMES PluginManager Utility
        PATH_SUFFIXES Map2X/Core
    )

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(MAP2X_CORE DEFAULT_MSG MAP2X_CORE_LIBRARY MAP2X_CORE_INCLUDE_DIR)

endif()
