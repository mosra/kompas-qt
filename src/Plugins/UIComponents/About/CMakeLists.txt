qt4_wrap_cpp(AboutUIComponent_MOC
    AboutUIComponent.h
    AboutDialog.h
)
kompas_add_static_plugin(KompasQt_Plugins AboutUIComponent
    AboutUIComponent.conf
    AboutUIComponent.cpp
    AboutDialog.cpp
    ${AboutUIComponent_MOC}
)
