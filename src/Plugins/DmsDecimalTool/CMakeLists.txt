include_directories("${Map2XQt_SOURCE_DIR}/src/")

set(Map2X_Qt_Plugins_DmsDecimalTool_SRCS
    DmsDecimalToolDialog.cpp
)

qt4_wrap_cpp(Map2X_Qt_Plugins_DmsDecimalTool_MOC
    DmsDecimalToolDialog.h
)

add_library(Map2XQtPluginsDmsDecimalTool STATIC ${Map2X_Qt_Plugins_DmsDecimalTool_SRCS} ${Map2X_Qt_Plugins_DmsDecimalTool_MOC})
target_link_libraries(Map2XQtPluginsDmsDecimalTool ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
