/*
    Copyright © 2007, 2008, 2009, 2010, 2011 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Kompas.

    Kompas is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Kompas is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include <QtCore/QTextCodec>
#include <QtCore/QTranslator>
#include <QtCore/QLocale>
#include <QtCore/QLibraryInfo>
#include <QtGui/QApplication>
#include "Utility/Translator.h"
#include "MainWindow.h"
#include "MainWindowConfigure.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    app.setApplicationName("Kompas");
    /** @todo Organization? */
    app.setOrganizationName("Mosra");

    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    /* Localizations */
    Corrade::Utility::Translator::setLocale(QLocale::system().name().toStdString());
    QTranslator translatorQt, translator;

    #ifndef _WIN32
    translatorQt.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    translator.load(QLocale::system().name(), TRANSLATION_DIR);
    #else
    /* On Win32 make the dir absolute */
    translatorQt.load("qt_" + QLocale::system().name(), QApplication::applicationDirPath() + TRANSLATION_DIR);
    translator.load(QLocale::system().name(), QApplication::applicationDirPath() + TRANSLATION_DIR);
    #endif

    app.installTranslator(&translatorQt);
    app.installTranslator(&translator);

    /* Main window */
    Kompas::QtGui::MainWindow w;
    w.show();

    return app.exec();
}
