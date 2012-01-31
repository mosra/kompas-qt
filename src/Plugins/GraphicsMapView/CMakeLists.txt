qt4_wrap_cpp(GraphicsMapView_MOC
    MapView.h
    GraphicsMapView.h
)
corrade_add_static_plugin(KompasQt_Plugins GraphicsMapView GraphicsMapView.conf
    MapView.cpp
    GraphicsMapView.cpp
    Tile.cpp
    ${GraphicsMapView_MOC}
)
