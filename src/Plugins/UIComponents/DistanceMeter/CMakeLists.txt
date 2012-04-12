qt4_wrap_cpp(DistanceMeterUIComponent_MOC
    DistanceMeterUIComponent.h
    DistanceMeterDialog.h
    CurrentCelestialBodyPluginModel.h
)
corrade_add_plugin(DistanceMeterUIComponent
    ${KOMPAS_PLUGINS_UICOMPONENT_INSTALL_DIR}
    DistanceMeterUIComponent.conf
    DistanceMeterUIComponent.cpp
    DistanceMeterDialog.cpp
    CurrentCelestialBodyPluginModel.cpp
    ${DistanceMeterUIComponent_MOC}
)

if(WIN32)
    target_link_libraries(DistanceMeterUIComponent ${KOMPAS_CORE_LIBRARY} ${KOMPAS_QT_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
endif()
