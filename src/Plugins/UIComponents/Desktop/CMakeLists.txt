qt4_wrap_cpp(DesktopUIComponent_MOC
    DesktopUIComponent.h
)
kompas_add_static_plugin(KompasQt_PluginsDesktop DesktopUIComponent
    DesktopUIComponent.conf
    DesktopUIComponent.cpp
    ${DesktopUIComponent_MOC}
)
