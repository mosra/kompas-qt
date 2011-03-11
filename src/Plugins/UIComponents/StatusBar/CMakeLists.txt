qt4_wrap_cpp(StatusBarUIComponent_MOC
    StatusBarUIComponent.h
)
kompas_add_static_plugin(KompasQt_PluginsDesktop StatusBarUIComponent
    StatusBarUIComponent.conf
    StatusBarUIComponent.cpp
    ${StatusBarUIComponent_MOC}
)
