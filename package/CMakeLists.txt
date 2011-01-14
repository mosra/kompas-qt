if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    install(FILES Kompas.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications/)
endif()
