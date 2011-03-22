# Don't display long long warnings for Qt
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/MainWindowConfigure.h.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/MainWindowConfigure.h)

include_directories(${KOMPAS_CORE_INCLUDE_DIR} ${KOMPAS_QT_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR})

add_subdirectory(Plugins)

set(Kompas_Qt_SRCS
    MainWindow.cpp
    AbstractMapView.cpp
    TileDataThread.cpp
    AbstractConfigurationDialog.cpp
    PluginModel.cpp
    LatLonCoordsEdit.cpp
    AbstractPluginMenuView.cpp
    OpenRasterMenuView.cpp
    PluginManagerStore.cpp
    RasterPackageModel.cpp
    RasterLayerModel.cpp
    RasterOverlayModel.cpp
    RasterZoomModel.cpp
    SessionManager.cpp
    MessageBox.cpp
)

qt4_wrap_cpp(Kompas_Qt_MOC
    MainWindow.h
    AbstractMapView.h
    TileDataThread.h
    AbstractConfigurationDialog.h
    AbstractConfigurationWidget.h
    AbstractPluginManager.h
    AbstractUIComponent.h
    PluginModel.h
    AbstractPluginMenuView.h
    RasterPackageModel.h
    RasterLayerModel.h
    RasterOverlayModel.h
    RasterZoomModel.h
    SessionManager.h
    LatLonCoordsEdit.h
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

# Qt library
add_library(KompasQt SHARED ${Kompas_Qt_SRCS} ${Kompas_Qt_MOC} ${Kompas_Qt_QRC})
target_link_libraries(KompasQt ${KOMPAS_CORE_LIBRARIES} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTNETWORK_LIBRARY})
set_target_properties(KompasQt PROPERTIES VERSION ${KOMPAS_QT_LIBRARY_VERSION} SOVERSION ${KOMPAS_QT_LIBRARY_SOVERSION})

# Desktop version
add_executable(kompas-qt main.cpp Plugins/registerStaticDesktop.cpp)
target_link_libraries(kompas-qt KompasQt ${KompasQt_Plugins} ${KompasQt_PluginsDesktop})

# Mobile version
add_executable(kompas-qt-mobile main.cpp Plugins/registerStaticMobile.cpp)
target_link_libraries(kompas-qt-mobile KompasQt ${KompasQt_Plugins} ${KompasQt_PluginsMobile})

install(TARGETS KompasQt DESTINATION ${KOMPAS_LIBRARY_INSTALL_DIR})
install(TARGETS kompas-qt DESTINATION ${KOMPAS_BINARY_INSTALL_DIR})
install(TARGETS kompas-qt-mobile DESTINATION ${KOMPAS_BINARY_INSTALL_DIR})

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
