add_definitions(-Wall -pedantic)

find_package(Qt4 REQUIRED)
find_package(Map2XCore REQUIRED)

# TODO: This will work only under user who compiled that
if(UNIX)
    set(CONFIGURATION_FILE "$ENV{HOME}/.map2x.conf")
    set(DATA_DIR "${CMAKE_INSTALL_PREFIX}/share/map2x/")
elseif(WIN32)
    set(CONFIGURATION_FILE "$ENV{AppData}/map2x.conf")
    set(DATA_DIR "${CMAKE_INSTALL_PREFIX}/Map2X/")
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/MainWindowConfigure.h.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/MainWindowConfigure.h)

include_directories(${MAP2X_CORE_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(Plugins)

set(Map2X_Qt_SRCS
    MainWindow.cpp
    AbstractMapView.cpp
    TileDataThread.cpp
    AbstractConfigurationDialog.cpp
    PluginModel.cpp
    PluginDialogTab.cpp
    PluginDialog.cpp
    main.cpp
)

qt4_wrap_cpp(Map2X_Qt_MOC
    MainWindow.h
    AbstractMapView.h
    TileDataThread.h
    AbstractConfigurationDialog.h
    AbstractConfigurationWidget.h
    PluginModel.h
    PluginDialogTab.h
    PluginDialog.h
)

qt4_add_resources(Map2X_Qt_QRC
    data/data.qrc
)

add_executable(map2x-qt ${Map2X_Qt_SRCS} ${Map2X_Qt_MOC} ${Map2X_Qt_QRC})
target_link_libraries(map2x-qt Map2XCore Map2XQtPlugins ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
