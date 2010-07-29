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

#include "AbstractConfigurationDialog.h"

#include <QtGui/QDialogButtonBox>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QMessageBox>

namespace Map2X { namespace QtGui {

AbstractConfigurationDialog::AbstractConfigurationDialog(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f) {
    /* Buttons */
    buttons = new QDialogButtonBox;
    restoreDefaultsButton = buttons->addButton(QDialogButtonBox::RestoreDefaults);
    resetButton = buttons->addButton(QDialogButtonBox::Reset);
    saveButton = buttons->addButton(QDialogButtonBox::Save);
    cancelButton = buttons->addButton(QDialogButtonBox::Cancel);

    connect(buttons, SIGNAL(accepted()), SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), SLOT(reject()));
    connect(restoreDefaultsButton, SIGNAL(clicked(bool)), this, SLOT(restoreDefaultsWarning()));

    /* Save and reset button is enabled only after editing, disable back after
        resets */
    resetButton->setDisabled(true);
    saveButton->setDisabled(true);
    connect(this, SIGNAL(restoreDefaults(bool)), resetButton, SLOT(setDisabled(bool)));
    connect(this, SIGNAL(restoreDefaults(bool)), saveButton, SLOT(setDisabled(bool)));
    connect(resetButton, SIGNAL(clicked(bool)), resetButton, SLOT(setEnabled(bool)));
    connect(resetButton, SIGNAL(clicked(bool)), saveButton, SLOT(setEnabled(bool)));

    /* Layout */
    _layout = new QVBoxLayout;
    _layout->addWidget(buttons);
    QWidget::setLayout(_layout);
}

void AbstractConfigurationDialog::setCentralWidget(QWidget* widget) {
    _layout->insertWidget(0, widget);
}

void AbstractConfigurationDialog::setCentralLayout(QLayout* layout) {
    _layout->insertLayout(0, layout);
}

void AbstractConfigurationDialog::connectWidget(AbstractConfigurationWidget* widget) {
    connect(widget, SIGNAL(edited(bool)), resetButton, SLOT(setEnabled(bool)));
    connect(widget, SIGNAL(edited(bool)), saveButton, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(accepted()), widget, SLOT(save()));
    connect(this, SIGNAL(restoreDefaults()), widget, SLOT(restoreDefaults()));
    connect(resetButton, SIGNAL(clicked(bool)), widget, SLOT(reset()));
}

void AbstractConfigurationDialog::restoreDefaultsWarning() {
    if(QMessageBox::warning(this, tr("Really restore defaults?"),
       tr("Do you really want to restore default configuration? This action is irreversible."),
       QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
        emit restoreDefaults();
}

}}
