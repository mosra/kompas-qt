add_definitions(-Wall -pedantic)

find_package(Qt4 REQUIRED)
find_package(Map2XCore REQUIRED)

include_directories(${MAP2X_CORE_INCLUDE_DIR})

add_subdirectory(Plugins)

set(Map2X_Qt_SRCS
    MainWindow.cpp
    AbstractMapView.cpp
    main.cpp
)

qt4_wrap_cpp(Map2X_Qt_MOC
    MainWindow.h
)

add_executable(map2x-qt ${Map2X_Qt_SRCS} ${Map2X_Qt_MOC})
target_link_libraries(map2x-qt Map2XCore Map2XQtPlugins ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
