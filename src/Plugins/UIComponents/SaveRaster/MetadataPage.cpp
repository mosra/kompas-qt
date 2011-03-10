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

#include "MetadataPage.h"

#include <QtCore/QFileInfo>
#include <QtGui/QGridLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QFileDialog>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

#include "SaveRasterWizard.h"
#include "MessageBox.h"
#include "MainWindow.h"

using namespace std;
using namespace Kompas::Core;
using namespace Kompas::QtGui;

namespace Kompas { namespace Plugins { namespace UIComponents {

MetadataPage::MetadataPage(SaveRasterWizard* _wizard): QWizardPage(_wizard), wizard(_wizard) {
    setTitle(tr("3/5: Metadata"));
    setSubTitle(tr("Select where to save the package and optionally fill in some metadata."));
    setPixmap(QWizard::LogoPixmap, QPixmap(":/progress3-48.png"));

    /* Initialize widgets */
    filename = new QLineEdit;
    connect(filename, SIGNAL(textChanged(QString)), SIGNAL(completeChanged()));
    name = new QLineEdit;
    description = new QLineEdit;
    packager = new QLineEdit;
    QPushButton* fileButton = new QPushButton(QIcon(":/open-16.png"), tr("Select..."));
    connect(fileButton, SIGNAL(clicked(bool)), SLOT(saveFileDialog()));

    QHBoxLayout* fileLayout = new QHBoxLayout;
    fileLayout->addWidget(filename);
    fileLayout->addWidget(fileButton);

    QGridLayout* layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Save to:")), 0, 0);
    layout->addLayout(fileLayout, 0, 1);
    layout->addWidget(new QLabel(tr("Package name:")), 1, 0);
    layout->addWidget(name, 1, 1);
    layout->addWidget(new QLabel(tr("Description:")), 2, 0);
    layout->addWidget(description, 2, 1);
    layout->addWidget(new QLabel(tr("Packager:")), 3, 0);
    layout->addWidget(packager, 3, 1);

    setLayout(layout);
}

bool MetadataPage::isComplete() const {
    if(filename->text().isEmpty()) return false;
    else return true;
}

bool MetadataPage::validatePage() {
    if(!checkSaveFile(filename->text())) return false;

    wizard->filename = filename->text().toStdString();
    wizard->name = name->text().toStdString();
    wizard->description = description->text().toStdString();
    wizard->packager = packager->text().toStdString();

    return true;
}

void MetadataPage::saveFileDialog(QString path) {
    /* Compose file extension filter, if available */
    QString extensions;
    if(!wizard->extensions.empty()) {
        extensions = QString::fromStdString(wizard->modelName) + " (";
        for(vector<string>::const_iterator it = wizard->extensions.begin(); it != wizard->extensions.end(); ++it)
            extensions += QString::fromStdString(*it) + ' ';
        extensions = extensions.left(extensions.length()-1) + ");;";
    }
    extensions += tr("All files") +  " (*)";

    /* Path where to open the dialog */
    if(path.isEmpty()) {
        if(filename->text().isEmpty())
            path = QString::fromStdString(MainWindow::instance()->configuration()->group("paths")->value<string>("packages"));
        else
            path = QFileInfo(filename->text()).absoluteDir().canonicalPath();
    }

    QString _filename = QFileDialog::getSaveFileName(this, tr("Save package as..."), path, extensions);

    if(_filename.isEmpty()) return;

    if(checkSaveFile(_filename)) filename->setText(_filename);
    else saveFileDialog(QFileInfo(_filename).absoluteDir().canonicalPath());
}

bool MetadataPage::checkSaveFile(const QString& filename) {
    if(filename.isEmpty()) return false;

    /* If the filename is the same as filename of any opened packages in source model, show error messagebox */
    Locker<const AbstractRasterModel> rasterModel = MainWindow::instance()->rasterModelForRead();
    bool is = false;
    for(int i = 0; i != rasterModel()->packageCount(); ++i) if(filename == QString::fromStdString(rasterModel()->packageAttribute(i, Kompas::Core::AbstractRasterModel::Filename))) {
        is = true;
        break;
    }
    rasterModel.unlock();

    if(is) {
        MessageBox::warning(this, tr("Saving to currently opened file"), tr("You selected file which is currently opened. Please select another file to avoid data loss."));
        return false;
    }

    /* If the format is multi-file, check that we are saving to clean directory */
    if(wizard->features & AbstractRasterModel::MultipleFileFormat) {
        QDir d(QFileInfo(filename).absoluteDir());
        QStringList l = d.entryList(QDir::AllEntries|QDir::NoDotAndDotDot);

        /* If the dir contains any file which isn't the saving file, error */
        if(l.count() > 1 || (l.count() == 1 && d.absoluteFilePath(l[0]) != filename)) {
            MessageBox::warning(this, tr("Empty directory needed"), tr("The package will be saved to multiple files. Please select empty directory to avoid data loss."));
            return false;
        }
    }

    return true;
}

}}}
