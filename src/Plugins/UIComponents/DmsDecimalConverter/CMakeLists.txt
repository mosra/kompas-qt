qt4_wrap_cpp(DmsDecimalConverterUIComponent_MOC
    DmsDecimalConverterUIComponent.h
    DmsDecimalConverterDialog.h
)
kompas_add_static_plugin(KompasQt_Plugins DmsDecimalConverterUIComponent
    DmsDecimalConverterUIComponent.conf
    DmsDecimalConverterUIComponent.cpp
    DmsDecimalConverterDialog.cpp
    ${DmsDecimalConverterUIComponent_MOC}
)
