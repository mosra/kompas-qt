qt4_wrap_cpp(StatusBarUIComponent_MOC
    StatusBarUIComponent.h
)
kompas_add_static_plugin(KompasQt_Plugins StatusBarUIComponent
    StatusBarUIComponent.conf
    StatusBarUIComponent.cpp
    ${StatusBarUIComponent_MOC}
)
