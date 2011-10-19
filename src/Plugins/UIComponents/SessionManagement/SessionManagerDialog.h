#ifndef Kompas_Plugins_UIComponents_SessionManagerDialog_h
#define Kompas_Plugins_UIComponents_SessionManagerDialog_h
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

#include <QtGui/QDialog>

class QListView;
class QPushButton;

namespace Kompas { namespace Plugins { namespace UIComponents {

class SessionManagerDialog: public QDialog {
    Q_OBJECT

    public:
        SessionManagerDialog(QWidget* parent = 0);

    private slots:
        void newEmpty();
        void saveAs();
        void saveAsDefault();
        void rename();
        void remove();
        void load();

        void updateButtons();

    private:
        QListView* view;

        QPushButton *saveAsButton,
            *renameButton,
            *removeButton,
            *loadButton;

        QAction* saveAsDefaultAction;

        QString newSessionName();
};

}}}

#endif
