include_directories("${Map2XQt_SOURCE_DIR}/src")

qt4_wrap_cpp(GraphicsMapView_MOC
    MapView.h
    GraphicsMapView.h
)
map2x_add_static_plugin(Map2XQt_Plugins GraphicsMapView GraphicsMapView.conf
    GraphicsMapView.cpp
    Tile.cpp
    ${GraphicsMapView_MOC}
)
