qt4_wrap_cpp(AboutUIComponent_MOC
    AboutUIComponent.h
    AboutDialog.h
)
corrade_add_static_plugin(KompasQt_PluginsDesktop AboutUIComponent
    AboutUIComponent.conf
    AboutUIComponent.cpp
    AboutDialog.cpp
    ${AboutUIComponent_MOC}
)
