include_directories("${Map2XQt_SOURCE_DIR}/src")

set(Map2X_Qt_Plugins_SRCS
    GraphicsMapView.cpp
    Tile.cpp
)

qt4_wrap_cpp(Map2X_Qt_Plugins_MOC
    MapView.h
    GraphicsMapView.h
)

add_library(Map2XQtPlugins STATIC ${Map2X_Qt_Plugins_SRCS} ${Map2X_Qt_Plugins_MOC})
target_link_libraries(Map2XQtPlugins Map2XQtPluginsDmsDecimalTool Map2XQtPluginsDistanceMeterTool ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTNETWORK_LIBRARY})

add_subdirectory(DmsDecimalTool)
add_subdirectory(DistanceMeterTool)
