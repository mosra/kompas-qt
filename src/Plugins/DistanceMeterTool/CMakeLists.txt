qt4_wrap_cpp(DistanceMeterTool_MOC
    DistanceMeterToolDialog.h
    CurrentCelestialBodyPluginModel.h
)
kompas_add_static_plugin(KompasQt_Plugins DistanceMeterTool DistanceMeterTool.conf
    DistanceMeterTool.cpp
    DistanceMeterToolDialog.cpp
    CurrentCelestialBodyPluginModel.cpp
    ${DistanceMeterTool_MOC}
)

if(WIN32)
    target_link_libraries(DistanceMeterTool ${KOMPAS_CORE_LIBRARIES})
endif()
