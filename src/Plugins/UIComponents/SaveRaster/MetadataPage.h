#ifndef Kompas_Plugins_UIComponents_MetadataPage_h
#define Kompas_Plugins_UIComponents_MetadataPage_h
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

/** @file
 * @brief Class Kompas::Plugins::UIComponents::MetadataPage
 */

#include <QtGui/QWizardPage>

class QLineEdit;

namespace Kompas { namespace Plugins { namespace UIComponents {

class SaveRasterWizard;

/**
 * @brief Metadata wizard page
 *
 * Allows user to specify package filename and package metadata (name,
 * description, packager name).
 */
class MetadataPage: public QWizardPage {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param _wizard       Wizard instance
         */
        MetadataPage(SaveRasterWizard* _wizard);

        /**
         * @brief Whether the page is complete
         * @return True if filename is not empty
         */
        virtual bool isComplete() const;

        /**
         * @brief Page validator
         *
         * Saves data to SaveRasterWizard::filename, SaveRasterWizard::name,
         * SaveRasterWizard::description and SaveRasterWizard::packager.
         */
        virtual bool validatePage();

    private slots:
        void saveFileDialog(QString path = "");

    private:
        SaveRasterWizard* wizard;

        QLineEdit *filename,
            *name,
            *description,
            *packager;

        bool checkSaveFile(const QString& filename);
};

}}}

#endif
