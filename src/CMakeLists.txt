set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")

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
    MovingWidget.cpp
    RasterPackageModel.cpp
    RasterLayerModel.cpp
    RasterOverlayModel.cpp
    RasterZoomModel.cpp
    SessionManager.cpp
    SessionMenuView.cpp
    MessageBox.cpp
    main.cpp
    Plugins/registerStatic.cpp
    ZoomSlider.cpp
)

qt4_wrap_cpp(Kompas_Qt_MOC
    AboutDialog.h
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
    MovingWidget.h
    RasterPackageModel.h
    RasterLayerModel.h
    RasterOverlayModel.h
    RasterZoomModel.h
    SessionManager.h
    SessionMenuView.h
    Wgs84CoordsEdit.h
    ZoomSlider.h
)

qt4_add_resources(Kompas_Qt_QRC
    ../graphics/data.qrc
)

# Diable console window, executable icon for WIN32
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows -Wl,-subsystem,windows")

    enable_language(RC)
    set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> -O coff <FLAGS> -o <OBJECT> -i <SOURCE>")
    set(Kompas_Qt_SRCS ${Kompas_Qt_SRCS} ../package/win-icon.rc)
endif(WIN32)

add_executable(kompas-qt ${Kompas_Qt_SRCS} ${Kompas_Qt_MOC} ${Kompas_Qt_QRC})
target_link_libraries(kompas-qt KompasCore ${KompasQt_Plugins} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTNETWORK_LIBRARY})

install(TARGETS kompas-qt DESTINATION ${KOMPAS_BINARY_INSTALL_DIR})

# Include also runtime libs for Win32 (remaining are bundled in Core)
if(WIN32)
    kompas_bundle_dlls(${KOMPAS_LIBRARY_INSTALL_DIR}
        libgcc_s_dw2-1.dll
        QtCore4.dll
        QtGui4.dll
        QtNetwork4.dll
        imageformats/qgif4.dll
        imageformats/qjpeg4.dll
        imageformats/qtiff4.dll
        PATHS ${DLL_SEARCH_PATH}
    )
endif()
