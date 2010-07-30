include_directories("${Map2XQt_SOURCE_DIR}/src/")

set(Map2X_Qt_Plugins_DistanceMeterTool_SRCS
    DistanceMeterToolDialog.cpp
)

qt4_wrap_cpp(Map2X_Qt_Plugins_DistanceMeterTool_MOC
    DistanceMeterToolDialog.h
)

add_library(Map2XQtPluginsDistanceMeterTool STATIC ${Map2X_Qt_Plugins_DistanceMeterTool_SRCS} ${Map2X_Qt_Plugins_DistanceMeterTool_MOC})
target_link_libraries(Map2XQtPluginsDistanceMeterTool ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
