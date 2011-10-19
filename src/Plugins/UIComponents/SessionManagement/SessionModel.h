#ifndef Kompas_Plugins_UIComponents_SessionModel_h
#define Kompas_Plugins_UIComponents_SessionModel_h
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

#include <QtCore/QAbstractListModel>

#include "MainWindow.h"
#include "SessionManager.h"

namespace Kompas { namespace Plugins { namespace UIComponents {

class SessionModel: public QAbstractListModel {
    Q_OBJECT

    public:
        SessionModel(QObject* parent = 0);

        inline int rowCount(const QModelIndex &parent = QModelIndex()) const {
            return QtGui::MainWindow::instance()->sessionManager()->names().count()+1;
        }
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    private slots:
        inline void reset() { QAbstractListModel::reset(); }
};

}}}

#endif
