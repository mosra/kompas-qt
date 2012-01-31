qt4_wrap_cpp(StatusBarUIComponent_MOC
    StatusBarUIComponent.h
)
corrade_add_static_plugin(KompasQt_PluginsDesktop StatusBarUIComponent
    StatusBarUIComponent.conf
    StatusBarUIComponent.cpp
    ${StatusBarUIComponent_MOC}
)
