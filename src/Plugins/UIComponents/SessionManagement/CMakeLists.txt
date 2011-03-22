qt4_wrap_cpp(SessionManagementUIComponent_MOC
    SessionManagementUIComponent.h
    SessionMenuView.h
)
kompas_add_static_plugin(KompasQt_PluginsDesktop SessionManagementUIComponent
    SessionManagementUIComponent.conf
    SessionManagementUIComponent.cpp
    SessionMenuView.cpp
    ${SessionManagementUIComponent_MOC}
)
