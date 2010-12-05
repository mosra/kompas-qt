add_definitions(-Wall -pedantic)

find_package(Qt4 REQUIRED)
find_package(KompasCore REQUIRED)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/MainWindowConfigure.h.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/MainWindowConfigure.h)

include_directories(${KOMPAS_CORE_INCLUDE_DIR} ${KOMPAS_QT_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(Plugins)

set(Kompas_Qt_SRCS
    AboutDialog.cpp
    MainWindow.cpp
    AbstractMapView.cpp
    TileDataThread.cpp
    AbstractConfigurationDialog.cpp
    PluginModel.cpp
    PluginDialog.cpp
    ConfigurationDialog.cpp
    Wgs84CoordsEdit.cpp
    AbstractPluginMenuView.cpp
    ToolPluginMenuView.cpp
    OpenRasterMenuView.cpp
    SaveRasterMenuView.cpp
    SaveRasterWizard.cpp
    SaveRasterThread.cpp
    MapOptionsDock.cpp
    RasterPackageModel.cpp
    RasterLayerModel.cpp
    RasterOverlayModel.cpp
    RasterZoomModel.cpp
    MessageBox.cpp
    main.cpp
    Plugins/registerStatic.cpp
)

qt4_wrap_cpp(Kompas_Qt_MOC
    MainWindow.h
    AbstractMapView.h
    TileDataThread.h
    AbstractConfigurationDialog.h
    AbstractConfigurationWidget.h
    AbstractPluginManager.h
    PluginModel.h
    PluginDialog.h
    ConfigurationDialog.h
    AbstractPluginMenuView.h
    SaveRasterWizard.h
    SaveRasterThread.h
    MapOptionsDock.h
    RasterPackageModel.h
    RasterLayerModel.h
    RasterOverlayModel.h
    RasterZoomModel.h
)

qt4_add_resources(Kompas_Qt_QRC
    data/data.qrc
)

add_executable(kompas-qt ${Kompas_Qt_SRCS} ${Kompas_Qt_MOC} ${Kompas_Qt_QRC})
target_link_libraries(kompas-qt KompasCore ${KompasQt_Plugins} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTNETWORK_LIBRARY})

install(TARGETS kompas-qt DESTINATION ${KOMPAS_BINARY_INSTALL_DIR})
