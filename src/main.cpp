/*
    Copyright © 2007, 2008, 2009, 2010 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Map2X.

    Map2X is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Map2X is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include <QtCore/QTextCodec>
#include <QtCore/QTranslator>
#include <QtCore/QLocale>
#include <QtCore/QLibraryInfo>
#include <QtGui/QApplication>

#include "MainWindow.h"
#include "PluginManager/PluginManager.h"
#include "Plugins/OpenStreetMapTileModel.h"
#include "Plugins/GraphicsMapView/GraphicsMapView.h"
#include "Plugins/DmsDecimalTool/DmsDecimalTool.h"
#include "Plugins/DistanceMeterTool/DistanceMeterTool.h"

int main(int argc, char** argv) {
    PLUGIN_IMPORT_STATIC(GraphicsMapView)
    PLUGIN_IMPORT_STATIC(OpenStreetMap)
    PLUGIN_IMPORT_STATIC(DmsDecimal)
    PLUGIN_IMPORT_STATIC(DistanceMeter)

    QApplication app(argc, argv);
    app.setApplicationName("Map2X");
    /** @todo Organization? */
    app.setOrganizationName("Mosra");

    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    /* Localizations */
    QTranslator translatorQt, translator;
    translatorQt.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&translatorQt);

    /* Main window */
    Map2X::QtGui::MainWindow w;
    w.show();

    return app.exec();
}
