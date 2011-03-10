qt4_wrap_cpp(MapOptionsUIComponent_MOC
    EditableRasterOverlayModel.h
    EditableRasterPackageModel.h
    MapOptionsDock.h
    MovingWidget.h
    ZoomSlider.h
)
kompas_add_static_plugin(KompasQt_Plugins MapOptionsUIComponent
    MapOptionsUIComponent.conf
    EditableRasterOverlayModel.cpp
    EditableRasterPackageModel.cpp
    MapOptionsUIComponent.cpp
    MapOptionsDock.cpp
    MovingWidget.cpp
    ZoomSlider.cpp
    ${MapOptionsUIComponent_MOC}
)
