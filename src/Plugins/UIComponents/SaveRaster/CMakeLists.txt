qt4_wrap_cpp(SaveRasterUIComponent_MOC
    AreaPage.h
    ContentsPage.h
    DownloadPage.h
    MetadataPage.h
    SaveRasterUIComponent.h
    SaveRasterMenuView.h
    SaveRasterThread.h
    SaveRasterWizard.h
    StatisticsPage.h
)
corrade_add_plugin(SaveRasterUIComponent
    ${KOMPAS_PLUGINS_UICOMPONENT_INSTALL_DIR}
    SaveRasterUIComponent.conf
    AreaPage.cpp
    ContentsPage.cpp
    DownloadPage.cpp
    MetadataPage.cpp
    SaveRasterUIComponent.cpp
    SaveRasterMenuView.cpp
    SaveRasterThread.cpp
    SaveRasterWizard.cpp
    StatisticsPage.cpp
    ${SaveRasterUIComponent_MOC}
)

if(WIN32)
    target_link_libraries(SaveRasterUIComponent ${KOMPAS_CORE_LIBRARY} ${KOMPAS_QT_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
endif()
