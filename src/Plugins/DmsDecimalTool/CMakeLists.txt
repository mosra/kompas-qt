qt4_wrap_cpp(DmsDecimalTool_MOC DmsDecimalToolDialog.h)
kompas_add_static_plugin(KompasQt_Plugins DmsDecimalTool DmsDecimalTool.conf
    DmsDecimalTool.cpp
    DmsDecimalToolDialog.cpp
    ${DmsDecimalTool_MOC}
)
