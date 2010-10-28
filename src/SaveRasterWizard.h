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
        SaveRasterWizard(QWidget* parent = 0, Qt::WindowFlags flags = 0);

    protected:
        class AreaPage;
        class ZoomPage;
        class LayersPage;
        class SavePage;
        class StatisticsPage;
        class DownloadPage;

        enum AreaType {
            WholeMap,
            VisibleArea,
            CustomArea
        };
        AreaType areaType;              /**< @brief Area type (from AreaPage) */

        /**
         * @brief List of zoom levels to save
         *
         * Sorted ascending.
         */
        QList<Core::Zoom> zoomLevels;

        QStringList layers,             /**< @brief Layers to save */
            overlays;                   /**< @brief Overlays to save */
};

class SaveRasterWizard::AreaPage: public QWizardPage {
    public:
        AreaPage(SaveRasterWizard* _wizard);

        virtual bool validatePage();

    private:
        SaveRasterWizard* wizard;

        QRadioButton *wholeMap,
            *visibleArea,
            *customArea;

        QPushButton* customAreaSelect;
};

class SaveRasterWizard::ZoomPage: public QWizardPage {
    Q_OBJECT

    public:
        ZoomPage(SaveRasterWizard* _wizard);

        virtual bool isComplete() const;
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

class SaveRasterWizard::LayersPage: public QWizardPage {
    public:
        LayersPage(SaveRasterWizard* _wizard);

        virtual bool isComplete() const;
        virtual bool validatePage();

    private:
        SaveRasterWizard* wizard;

        QListView *layersView,
            *overlaysView;
};

class SaveRasterWizard::StatisticsPage: public QWizardPage {
    public:
        StatisticsPage(SaveRasterWizard* _wizard);

        inline virtual bool isComplete() const { return canDownload; }
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

class SaveRasterWizard::DownloadPage: public QWizardPage {
    public:
        DownloadPage(SaveRasterWizard* _wizard);

    private:
        SaveRasterWizard* wizard;
};

}}

#endif
