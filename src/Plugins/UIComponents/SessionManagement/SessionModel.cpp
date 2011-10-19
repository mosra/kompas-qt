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

#include "SessionModel.h"

using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

SessionModel::SessionModel(QObject* parent): QAbstractListModel(parent) {
    connect(MainWindow::instance()->sessionManager(), SIGNAL(namesChanged()), SLOT(reset()));
    connect(MainWindow::instance()->sessionManager(), SIGNAL(currentChanged(uint)), SLOT(reset()));
}

QVariant SessionModel::data(const QModelIndex& index, int role) const {
    if(role == Qt::DisplayRole) {
        if(index.row() == 0)
            return tr("(default session)");
        return MainWindow::instance()->sessionManager()->names()[index.row()-1];
    } else if(role == Qt::FontRole && static_cast<unsigned int>(index.row()) == MainWindow::instance()->sessionManager()->current()) {
        QFont font;
        font.setBold(true);
        return font;
    }

    return QVariant();
}

}}}
