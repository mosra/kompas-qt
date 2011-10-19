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

#include "SessionManagerDialog.h"

#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QInputDialog>
#include <QtGui/QListView>
#include <QtGui/QMenu>
#include <QtGui/QPushButton>

#include "MainWindow.h"
#include "MessageBox.h"
#include "SessionModel.h"

using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

SessionManagerDialog::SessionManagerDialog(QWidget* parent): QDialog(parent) {
    /* Session list */
    SessionModel* model = new SessionModel(this);
    view = new QListView;
    view->setModel(model);
    connect(view->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(updateButtons()));
    connect(model, SIGNAL(modelReset()), SLOT(updateButtons()));

    /* New empty session */
    QPushButton* newEmptyButton = new QPushButton(tr("New empty session"));
    connect(newEmptyButton, SIGNAL(clicked(bool)), SLOT(newEmpty()));

    /* Save as / save as default */
    saveAsButton = new QPushButton(tr("Save as..."));
    QMenu* saveAsMenu = new QMenu(saveAsButton);
    saveAsMenu->addAction(tr("New session"), this, SLOT(saveAs()));
    saveAsDefaultAction = saveAsMenu->addAction(tr("Default session"), this, SLOT(saveAsDefault()));
    saveAsButton->setMenu(saveAsMenu);

    /* Rename session */
    renameButton = new QPushButton(tr("Rename"));
    connect(renameButton, SIGNAL(clicked(bool)), SLOT(rename()));

    /* Delete dession */
    removeButton = new QPushButton(tr("Delete"));
    connect(removeButton, SIGNAL(clicked(bool)), SLOT(remove()));

    /* Load session */
    loadButton = new QPushButton(tr("Load"));
    connect(loadButton, SIGNAL(clicked(bool)), SLOT(load()));

    QDialogButtonBox* buttons = new QDialogButtonBox(Qt::Vertical);
    buttons->addButton(newEmptyButton, QDialogButtonBox::ActionRole);
    buttons->addButton(saveAsButton, QDialogButtonBox::ActionRole);
    buttons->addButton(renameButton, QDialogButtonBox::ActionRole);
    buttons->addButton(removeButton, QDialogButtonBox::ActionRole);
    buttons->addButton(loadButton, QDialogButtonBox::ActionRole);
    buttons->addButton(QDialogButtonBox::Close);
    connect(buttons, SIGNAL(rejected()), SLOT(accept()));

    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(view, 1);
    layout->addWidget(buttons);
    setLayout(layout);

    /* Disable buttons */
    updateButtons();

    setWindowTitle(tr("Session manager"));
    resize(380, 320);
    setAttribute(Qt::WA_DeleteOnClose);
}

void SessionManagerDialog::newEmpty() {
    bool ok;
    QString name = QInputDialog::getText(this, tr("New session"), tr("Enter new session name:"), QLineEdit::Normal, newSessionName(), &ok);
    if(!ok) return;

     MainWindow::instance()->sessionManager()->newSession(name);
}

void SessionManagerDialog::saveAs() {
    bool ok;
    QString name = QInputDialog::getText(this, tr("New session"), tr("Enter new session name:"), QLineEdit::Normal, newSessionName(), &ok);
    if(!ok) return;

    unsigned int source = view->currentIndex().row();
    unsigned int destination = MainWindow::instance()->sessionManager()->newSession(name);
    Utility::Debug() << source << destination;
    MainWindow::instance()->sessionManager()->copySession(source, destination);
}

void SessionManagerDialog::saveAsDefault() {
    MainWindow::instance()->sessionManager()->copySession(view->currentIndex().row(), 0);
}

void SessionManagerDialog::rename() {
    int current = view->currentIndex().row();

    bool ok;
    QString name = QInputDialog::getText(this, tr("Rename session"), tr("Enter new session name:"), QLineEdit::Normal, MainWindow::instance()->sessionManager()->names()[current-1], &ok);
    if(!ok) return;

    MainWindow::instance()->sessionManager()->renameSession(current, name);
}

void SessionManagerDialog::remove() {
    int current = view->currentIndex().row();

    if(MessageBox::question(this, tr("Delete session"), tr("Are you sure you want to delete session '%0'?").arg(MainWindow::instance()->sessionManager()->names()[current-1]), QMessageBox::Yes|QMessageBox::No, QMessageBox::No) != QMessageBox::Yes) return;

    MainWindow::instance()->sessionManager()->deleteSession(current);
}

void SessionManagerDialog::load() {
    MainWindow::instance()->sessionManager()->load(view->currentIndex().row());
}

void SessionManagerDialog::updateButtons() {
    /* Enable Save As and Load buttons only if something is selected */
    bool disabled = !view->selectionModel()->hasSelection();
    saveAsButton->setDisabled(disabled);
    loadButton->setDisabled(disabled);

    /* Disable rename, remove and save as default buttons for default session */
    bool isDefault = view->currentIndex().row() == 0;
    renameButton->setDisabled(disabled || isDefault);
    saveAsDefaultAction->setDisabled(disabled || isDefault);

    /* Disable remove and load button for currently loaded session */
    bool isLoaded = static_cast<unsigned int>(view->selectionModel()->currentIndex().row()) == MainWindow::instance()->sessionManager()->current();
    removeButton->setDisabled(disabled || isDefault || isLoaded);
    loadButton->setDisabled(disabled || isLoaded);
}

QString SessionManagerDialog::newSessionName() {
    QString base(tr("New session"));
    QString baseNumbered(tr("New session (%0)"));

    if(MainWindow::instance()->sessionManager()->names().contains(base)) {
        int number = 1;

        do {
            ++number;
        } while(MainWindow::instance()->sessionManager()->names().contains(baseNumbered.arg(number)));

        return baseNumbered.arg(number);
    }

    return base;
}

}}}
