/*
    Copyright © 2007, 2008, 2009, 2010 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Kompas.

    Kompas is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Kompas is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "MessageBox.h"

namespace Kompas { namespace QtGui {

QMessageBox::StandardButton MessageBox::information(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton) {
    MessageBox b(Information, title, text, buttons, parent);
    b.setDefaultButton(defaultButton);
    b.exec();
    return b.standardButton(b.clickedButton());
}

QMessageBox::StandardButton MessageBox::question(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton) {
    MessageBox b(Question, title, text, buttons, parent);
    b.setDefaultButton(defaultButton);
    b.exec();
    return b.standardButton(b.clickedButton());
}


QMessageBox::StandardButton MessageBox::warning(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton) {
    MessageBox b(Warning, title, text, buttons, parent);
    b.setDefaultButton(defaultButton);
    b.exec();
    return b.standardButton(b.clickedButton());
}

QMessageBox::StandardButton MessageBox::critical(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton) {
    MessageBox b(Critical, title, text, buttons, parent);
    b.setDefaultButton(defaultButton);
    b.exec();
    return b.standardButton(b.clickedButton());
}

void MessageBox::setIcon(QMessageBox::Icon icon) {
    switch(icon) {
        /** @todo Do a Question icon */
//      case Question:
//          setIconPixmap(QPixmap(":/messagebox-question-64.png")); break;
        case Information:
            setIconPixmap(QPixmap(":/messagebox-information-64.png")); break;
        case Warning:
            setIconPixmap(QPixmap(":/messagebox-warning-64.png")); break;
        case Critical:
            setIconPixmap(QPixmap(":/messagebox-error-64.png")); break;
        default:
            QMessageBox::setIcon(icon);
    };
}

}}
