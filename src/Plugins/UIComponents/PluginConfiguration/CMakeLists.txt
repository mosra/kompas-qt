qt4_wrap_cpp(PluginConfigurationUIComponent_MOC
    PluginConfigurationUIComponent.h
    PluginDialog.h
    PluginTab.h
)
corrade_add_static_plugin(KompasQt_Plugins PluginConfigurationUIComponent
    PluginConfigurationUIComponent.conf
    PluginConfigurationUIComponent.cpp
    PluginDialog.cpp
    PluginTab.cpp
    ${PluginConfigurationUIComponent_MOC}
)
