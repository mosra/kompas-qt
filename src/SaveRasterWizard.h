#ifndef Kompas_QtGui_SaveRasterWizard_h
#define Kompas_QtGui_SaveRasterWizard_h
/*
    Copyright © 2007, 2008, 2009, 2010 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Kompas::QtGui::SaveRasterWizard
 */

#include <QtGui/QWizard>
#include <QtGui/QSpinBox>

#include "AbstractRasterModel.h"
#include "AbsoluteArea.h"

class QListView;
class QRadioButton;
class QLabel;
class QGroupBox;
class QProgressBar;

namespace Kompas { namespace QtGui {

class SaveRasterThread;

/**
 * @brief Wizard for saving raster maps
 * @todo @c VERSION-0.2 Final package size
 */
class SaveRasterWizard: public QWizard {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param _model        Model to which save
         * @param parent        Parent widget
         * @param flags         Window flags
         */
        SaveRasterWizard(const std::string& _model, QWidget* parent = 0, Qt::WindowFlags flags = 0);

    public slots:
        /**
         * @brief Show the dialog
         *
         * Checks whether destination model supports all features provided from
         * source model, if not, asks user whether to continue.
         */
        int exec();

    protected:
        class AreaPage;
        class ContentsPage;
        class SavePage;
        class StatisticsPage;
        class MetadataPage;
        class DownloadPage;

        std::string model;              /**< @brief Destination model */
        std::string modelName;          /**< @brief Translated model name */
        int features;                   /**< @brief Features of destination model */
        std::vector<std::string> extensions; /**< @brief File extensions of destination model */

        Core::TileSize tileSize;        /**< @brief Tile size of source model */
        Core::AbsoluteArea<double> absoluteArea; /**< @brief Area to download */

        /**
         * @brief List of zoom levels to save
         */
        std::vector<Core::Zoom> zoomLevels;
        std::vector<std::string>
            layers,                     /**< @brief Layers to save */
            overlays;                   /**< @brief Overlays to save */

        std::string filename,           /**< @brief Package filename */
            name,                       /**< @brief Package name */
            description,                /**< @brief Package description */
            packager;                   /**< @brief Packager name */

        bool openWhenFinished;          /**< @brief Whether to open the package when finished */

        /**
         * @brief Tile area at minimal zoom
         *
         * Computed from absoluteArea, model area, model minimal zoom and
         * zoomLevels.
         */
        Core::TileArea area() const;

        virtual void done(int result);
};

/**
 * @brief Area selection wizard page
 *
 * Provides selection of whole map, visible area or custom specified area.
 * @see SaveRasterWizard::AreaType
 */
class SaveRasterWizard::AreaPage: public QWizardPage {
    Q_OBJECT

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
 * @brief Map contents wizard page
 *
 * Provides three listviews for selecting zoom levels, layers and overlays.
 */
class SaveRasterWizard::ContentsPage: public QWizardPage {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param _wizard       Wizard instance
         */
        ContentsPage(SaveRasterWizard* _wizard);

        /**
         * @brief Whether the page is complete
         * @return True if at least one zoom level and layer is selected.
         */
        virtual bool isComplete() const;

        /**
         * @brief Page validator
         *
         * Saves selected zoom levels into SaveRasterWizard::zoomLevels,
         * layers into SaveRasterWizard::layers and overlays into
         * SaveRasterWizard::overlays.
         */
        virtual bool validatePage();

    private:
        SaveRasterWizard* wizard;

        QListView *zoomLevelsView,
            *layersView,
            *overlaysView;
};

/**
 * @brief Statistics wizard page
 *
 * Displays total tile count and estimated package size.
 */
class SaveRasterWizard::StatisticsPage: public QWizardPage {
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

/**
 * @brief Metadata wizard page
 *
 * Allows user to specify package filename and package metadata (name,
 * description, packager name).
 */
class SaveRasterWizard::MetadataPage: public QWizardPage {
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

/**
 * @brief Download wizard page
 *
 * Downloads all tile data and creates the package.
 */
class SaveRasterWizard::DownloadPage: public QWizardPage {
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

        inline void setOpenWhenFinished(bool open) { wizard->openWhenFinished = open; }

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

}}

#endif
