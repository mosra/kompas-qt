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

if(WIN32)
    target_link_libraries(GraphicsMapView ${KOMPAS_QT_LIBRARY})
endif()
