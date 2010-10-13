qt4_wrap_cpp(DmsDecimalTool_MOC DmsDecimalToolDialog.h)
map2x_add_static_plugin(Map2XQt_Plugins DmsDecimalTool DmsDecimalTool.conf
    DmsDecimalTool.cpp
    DmsDecimalToolDialog.cpp
    ${DmsDecimalTool_MOC}
)
