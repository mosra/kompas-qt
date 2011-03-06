qt4_wrap_cpp(ConfigurationUIComponent_MOC
    ConfigurationUIComponent.h
    ConfigurationDialog.h
    ConfigurationWidget.h
)
kompas_add_static_plugin(KompasQt_Plugins ConfigurationUIComponent
    ConfigurationUIComponent.conf
    ConfigurationUIComponent.cpp
    ConfigurationDialog.cpp
    ConfigurationWidget.cpp
    ${ConfigurationUIComponent_MOC}
)
