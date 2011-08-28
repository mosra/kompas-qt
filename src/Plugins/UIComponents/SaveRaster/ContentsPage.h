#ifndef Kompas_Plugins_UIComponents_ContentsPage_h
#define Kompas_Plugins_UIComponents_ContentsPage_h
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
 * @brief Class Kompas::Plugins::UIComponents::ContentsPage
 */

#include <QtGui/QWizardPage>

class QListView;

namespace Kompas { namespace Plugins { namespace UIComponents {

class SaveRasterWizard;

/**
 * @brief Map contents wizard page
 *
 * Provides three listviews for selecting zoom levels, layers and overlays.
 */
class ContentsPage: public QWizardPage {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param _wizard       Wizard instance
         */
        ContentsPage(SaveRasterWizard* _wizard);

        /**
         * @brief Page initializator
         *
         * Enables or disables multiselection based on destination model
         * features.
         */
        void initializePage();

        /**
         * @brief Whether the page is complete
         * @return True if at least one zoom level and layer is selected.
         */
        bool isComplete() const;

        /**
         * @brief Page validator
         *
         * Saves selected zoom levels into SaveRasterWizard::zoomLevels,
         * layers into SaveRasterWizard::layers and overlays into
         * SaveRasterWizard::overlays.
         */
        bool validatePage();

    private:
        SaveRasterWizard* wizard;

        QListView *zoomLevelsView,
            *layersView,
            *overlaysView;
};

}}}

#endif
