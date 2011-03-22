qt4_wrap_cpp(MobileUIComponent_MOC
    MobileUIComponent.h
)
kompas_add_static_plugin(KompasQt_PluginsMobile MobileUIComponent
    MobileUIComponent.conf
    MobileUIComponent.cpp
    ${MobileUIComponent_MOC}
)
