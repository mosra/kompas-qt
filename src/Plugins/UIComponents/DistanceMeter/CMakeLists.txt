qt4_wrap_cpp(DistanceMeterUIComponent_MOC
    DistanceMeterUIComponent.h
    DistanceMeterDialog.h
    CurrentCelestialBodyPluginModel.h
)
kompas_add_static_plugin(KompasQt_Plugins DistanceMeterUIComponent
    DistanceMeterUIComponent.conf
    DistanceMeterUIComponent.cpp
    DistanceMeterDialog.cpp
    CurrentCelestialBodyPluginModel.cpp
    ${DistanceMeterUIComponent_MOC}
)

if(WIN32)
    target_link_libraries(DistanceMeterUIComponent ${KOMPAS_CORE_LIBRARIES})
endif()
