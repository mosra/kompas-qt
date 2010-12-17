qt4_add_translation(Kompas_QM
    cs_CZ.ts
)

add_custom_target(KompasLocalization ALL DEPENDS ${Kompas_QM})

install(FILES ${Kompas_QM} DESTINATION ${KOMPAS_QT_TRANSLATION_INSTALL_DIR})
