qt4_wrap_cpp(SessionManagementUIComponent_MOC
    SessionManagementUIComponent.h
    SessionManagerDialog.h
    SessionMenuView.h
    SessionModel.h
)
kompas_add_static_plugin(KompasQt_PluginsDesktop SessionManagementUIComponent
    SessionManagementUIComponent.conf
    SessionManagementUIComponent.cpp
    SessionManagerDialog.cpp
    SessionMenuView.cpp
    SessionModel.cpp
    ${SessionManagementUIComponent_MOC}
)
