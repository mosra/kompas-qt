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

#include "AboutDialog.h"

#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QScrollArea>

namespace Kompas { namespace QtGui {

AboutDialog::AboutDialog(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f) {
    QLabel* brief = new QLabel(QString(
        "<span style=\"font-size: 32px; font-weight: bold;\">Kompas</span> %0<br/>"
        "%1<br /><br />"
        "<a href=\"http://mosra.cz/blog/kompas.php\">http://mosra.cz/blog/kompas.php</a><br />"
        "%2")
        .arg("0.1")
        .arg(tr("Portable navigation system."))
        .arg(tr("Licensed under GNU LGPL version 3.")));
    brief->setAlignment(Qt::AlignTop);
    brief->setWordWrap(true);
    brief->setOpenExternalLinks(true);

    QString item("%0<div style=\"margin-left: 20px;\"><a href=\"mailto:%1\">%1</a><br />%2</div><br />");

    QLabel* authors = new QLabel(
        item.arg("Vladimír Vondruš").arg("mosra@centrum.cz").arg(tr("Main developer, © 2007-2010.")) +
        item.arg("Jan Dupal").arg("dupal.j@seznam.cz").arg(tr("Coordinate conversion algorithms, tool plugins.")));
    authors->setAlignment(Qt::AlignTop);
    authors->setWordWrap(true);
    authors->setOpenExternalLinks(true);

    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidget(authors);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setWidgetResizable(true);

    QLabel* icon = new QLabel;
    icon->setPixmap(QPixmap(":/logo-64.png"));
    icon->setAlignment(Qt::AlignCenter);
    icon->setFixedSize(80, 80);

    QGridLayout* layout = new QGridLayout;
    layout->addWidget(icon, 0, 0);
    layout->addWidget(brief, 0, 1);
    layout->addWidget(new QLabel(QString("<br />%0").arg(tr("Authors:"))), 1, 0, 1, 2);
    layout->addWidget(scrollArea, 2, 0, 1, 2);
    setLayout(layout);

    setWindowTitle(tr("About Kompas"));
    setFixedSize(320, 280);
    setAttribute(Qt::WA_DeleteOnClose);
}

}}
