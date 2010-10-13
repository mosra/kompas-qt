qt4_wrap_cpp(DistanceMeterTool_MOC DistanceMeterToolDialog.h)
map2x_add_static_plugin(Map2XQt_Plugins DistanceMeterTool DistanceMeterTool.conf
    DistanceMeterTool.cpp
    DistanceMeterToolDialog.cpp
    ${DistanceMeterTool_MOC}
)
