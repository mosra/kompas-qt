#ifndef Map2X_QtGui_SaveRasterWizard_h
#define Map2X_QtGui_SaveRasterWizard_h
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

/** @file
 * @brief Class Map2X::QtGui::SaveRasterWizard
 */

#include <QtGui/QWizard>
#include <QtGui/QSpinBox>

#include "AbstractRasterModel.h"

class QListView;
class QRadioButton;
class QLabel;
class QGroupBox;

namespace Map2X { namespace QtGui {

/** @brief Wizard for saving raster maps */
class SaveRasterWizard: public QWizard {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param parent        Parent widget
         * @param flags         Window flags
         */
        SaveRasterWizard(QWidget* parent = 0, Qt::WindowFlags flags = 0);

    protected:
        class AreaPage;
        class ZoomPage;
        class LayersPage;
        class SavePage;
        class StatisticsPage;
        class DownloadPage;

        /** @brief Area type (from AreaPage) */
        enum AreaType {
            WholeMap,
            VisibleArea,
            CustomArea
        };

        /**
         * @brief Area type (from AreaPage)
         * @see SaveRasterWizard::AreaPage::validatePage()
         */
        AreaType areaType;

        /**
         * @brief List of zoom levels to save
         *
         * Sorted ascending.
         * @see SaveRasterWizard::ZoomPage::validatePage()
         */
        QList<Core::Zoom> zoomLevels;

        QStringList layers,             /**< @brief Layers to save */
            overlays;                   /**< @brief Overlays to save */
};

/**
 * @brief Area selection wizard page
 *
 * Provides selection of whole map, visible area or custom specified area.
 * @see SaveRasterWizard::AreaType
 */
class SaveRasterWizard::AreaPage: public QWizardPage {
    public:
        /**
         * @brief Constructor
         * @param _wizard       Wizard instance
         */
        AreaPage(SaveRasterWizard* _wizard);

        /**
         * @brief Page validator
         *
         * Saves selected area type into SaveRasterWizard::areaType.
         */
        virtual bool validatePage();

    private:
        SaveRasterWizard* wizard;

        QRadioButton *wholeMap,
            *visibleArea,
            *customArea;

        QPushButton* customAreaSelect;
};

/**
 * @brief Zoom selection wizard page
 *
 * Provides selection of zoom levels as a range or by selecting particular zoom
 * levels.
 */
class SaveRasterWizard::ZoomPage: public QWizardPage {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param _wizard       Wizard instance
         */
        ZoomPage(SaveRasterWizard* _wizard);

        /**
         * @brief Whether the page is complete
         * @return True if at least one zoom level is selected
         */
        virtual bool isComplete() const;

        /**
         * @brief Page validator
         *
         * Saves selected zoom levels into SaveRasterWizard::zoomLevels.
         */
        virtual bool validatePage();

    private slots:
        inline void checkMinValue(int value) {
            if(maxZoom->value() < minZoom->value())
                maxZoom->setValue(minZoom->value());
        }
        inline void checkMaxValue(int value) {
            if(minZoom->value() > maxZoom->value())
                minZoom->setValue(maxZoom->value());
        }
        void switchGroups();

    private:
        SaveRasterWizard* wizard;

        QSpinBox *minZoom,
            *maxZoom;
        QGroupBox *basic,
            *advanced;
        QRadioButton *basicButton,
            *advancedButton;
        QListView *zoomLevelsView;
};

/**
 * @brief Layer and overlay selection wizard page
 *
 * Provides two listviews for selecting layers and overlays.
 */
class SaveRasterWizard::LayersPage: public QWizardPage {
    public:
        /**
         * @brief Constructor
         * @param _wizard       Wizard instance
         */
        LayersPage(SaveRasterWizard* _wizard);

        /**
         * @brief Whether the page is complete
         * @return True if at least one layer is selected.
         */
        virtual bool isComplete() const;

        /**
         * @brief Page validator
         *
         * Saves selected layers into SaveRasterWizard::layers and overlays into
         * SaveRasterWizard::overlays.
         */
        virtual bool validatePage();

    private:
        SaveRasterWizard* wizard;

        QListView *layersView,
            *overlaysView;
};

/**
 * @brief Statistics wizard page
 *
 * Displays total tile count and estimated package size.
 */
class SaveRasterWizard::StatisticsPage: public QWizardPage {
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

/**
 * @brief Download wizard page
 *
 * Downloads all tile data and creates the package.
 */
class SaveRasterWizard::DownloadPage: public QWizardPage {
    public:
        /**
         * @brief Constructor
         * @param _wizard       Wizard instance
         */
        DownloadPage(SaveRasterWizard* _wizard);

    private:
        SaveRasterWizard* wizard;
};

}}

#endif
