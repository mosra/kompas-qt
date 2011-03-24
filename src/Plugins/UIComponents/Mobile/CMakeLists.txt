qt4_wrap_cpp(MobileUIComponent_MOC
    MobileUIComponent.h
    MobileCentralWidget.h
)
kompas_add_static_plugin(KompasQt_PluginsMobile MobileUIComponent
    MobileUIComponent.conf
    MobileUIComponent.cpp
    MobileCentralWidget.cpp
    ${MobileUIComponent_MOC}
)
