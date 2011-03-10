#ifndef Kompas_Plugins_UIComponents_StatisticsPage_h
#define Kompas_Plugins_UIComponents_StatisticsPage_h
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
 * @brief Class Kompas::Plugins::UIComponents::StatisticsPage
 */

#include <QtGui/QWizardPage>

class QLabel;

namespace Kompas { namespace Plugins { namespace UIComponents {

class SaveRasterWizard;

/**
 * @brief Statistics wizard page
 *
 * Displays total tile count and estimated package size.
 */
class StatisticsPage: public QWizardPage {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param _wizard       Wizard instance
         */
        StatisticsPage(SaveRasterWizard* _wizard);

        /**
         * @brief Whether the page is complete
         * @return True if estimated package size doesn't exceed 10 GB.
         */
        inline virtual bool isComplete() const { return canDownload; }

        /**
         * @brief Page initializator
         *
         * Gets all needed data from SaveRasterWizard and computes data count.
         */
        virtual void initializePage();

    private:
        SaveRasterWizard* wizard;

        bool canDownload;

        QLabel *tileCountMinZoom,
            *zoomLevelCount,
            *tileCountOneLayer,
            *layerCount,
            *tileCountTotal,
            *downloadSize,
            *fupWarning;
};

}}}

#endif
