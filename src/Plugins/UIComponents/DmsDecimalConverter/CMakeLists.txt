qt4_wrap_cpp(DmsDecimalConverterUIComponent_MOC
    DmsDecimalConverterUIComponent.h
    DmsDecimalConverterDialog.h
)
corrade_add_plugin(DmsDecimalConverterUIComponent
    ${KOMPAS_PLUGINS_UICOMPONENT_INSTALL_DIR}
    DmsDecimalConverterUIComponent.conf
    DmsDecimalConverterUIComponent.cpp
    DmsDecimalConverterDialog.cpp
    ${DmsDecimalConverterUIComponent_MOC}
)

if(WIN32)
    target_link_libraries(DmsDecimalConverterUIComponent ${KOMPAS_CORE_LIBRARY} ${KOMPAS_QT_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
endif()
