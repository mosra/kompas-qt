include_directories("${Map2XQt_SOURCE_DIR}/src")

set(Map2X_Qt_Plugins_SRCS
    GraphicsMapView.cpp
    Tile.cpp
)

qt4_wrap_cpp(Map2X_Qt_Plugins_MOC
    MapView.h
    GraphicsMapView.h
)

add_library(Map2XQtPluginsGraphicsMapView STATIC ${Map2X_Qt_Plugins_SRCS} ${Map2X_Qt_Plugins_MOC})
target_link_libraries(Map2XQtPluginsGraphicsMapView ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
