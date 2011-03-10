#ifndef Kompas_Plugins_UIComponents_DownloadPage_h
#define Kompas_Plugins_UIComponents_DownloadPage_h
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
 * @brief Class Kompas::Plugins::UIComponents::DownloadPage
 */

#include <QtGui/QWizardPage>

#include "AbstractRasterModel.h"

class QLabel;
class QProgressBar;

namespace Kompas { namespace Plugins { namespace UIComponents {

class SaveRasterWizard;
class SaveRasterThread;

/**
 * @brief Download wizard page
 *
 * Downloads all tile data and creates the package.
 */
class DownloadPage: public QWizardPage {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param _wizard       Wizard instance
         */
        DownloadPage(SaveRasterWizard* _wizard);

        /**
         * @brief Page initializer
         *
         * Starts downloading immediately.
         */
        virtual void initializePage();

        /**
         * @brief Whether the page is complete
         * @return True if download is finished
         */
        inline virtual bool isComplete() const { return _isComplete; }

    private slots:
        void updateStatus(Core::Zoom _currentZoom, int currentZoomNumber, const std::string& _currentLayer, int currentLayerNumber, int _totalCompleted, int _currentZoomLayerCompleted);

        void completed();
        void error();

        void setOpenWhenFinished(bool open);

    private:
        SaveRasterWizard* wizard;
        SaveRasterThread* saveThread;

        QLabel *filename,
            *currentZoom,
            *currentLayer;

        QProgressBar *totalCompleted,
            *currentZoomLayerCompleted;

        bool _isComplete;
};

}}}

#endif
