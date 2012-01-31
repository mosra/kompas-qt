qt4_wrap_cpp(ConfigurationUIComponent_MOC
    CacheTab.h
    ConfigurationUIComponent.h
    ConfigurationDialog.h
    MainTab.h
)
corrade_add_static_plugin(KompasQt_Plugins ConfigurationUIComponent
    ConfigurationUIComponent.conf
    CacheTab.cpp
    ConfigurationUIComponent.cpp
    ConfigurationDialog.cpp
    MainTab.cpp
    ${ConfigurationUIComponent_MOC}
)
