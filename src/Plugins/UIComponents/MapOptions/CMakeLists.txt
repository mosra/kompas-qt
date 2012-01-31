qt4_wrap_cpp(MapOptionsUIComponent_MOC
    EditableRasterOverlayModel.h
    EditableRasterPackageModel.h
    MapOptionsUIComponent.h
    MapOptionsDock.h
    MovingWidget.h
    ZoomSlider.h
)
corrade_add_static_plugin(KompasQt_PluginsDesktop MapOptionsUIComponent
    MapOptionsUIComponent.conf
    EditableRasterOverlayModel.cpp
    EditableRasterPackageModel.cpp
    MapOptionsUIComponent.cpp
    MapOptionsDock.cpp
    MovingWidget.cpp
    ZoomSlider.cpp
    ${MapOptionsUIComponent_MOC}
)
