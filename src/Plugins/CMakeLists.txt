include_directories("${Map2XQt_SOURCE_DIR}/src")

set(Map2X_Qt_Plugins_SRCS
    MapView.cpp
)

qt4_wrap_cpp(Map2X_Qt_Plugins_MOC
    MapView.h
)

add_library(Map2XQtPlugins STATIC ${Map2X_Qt_Plugins_SRCS} ${Map2X_Qt_Plugins_MOC})
