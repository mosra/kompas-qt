#ifndef Kompas_Plugins_UIComponents_SaveRasterWizard_h
#define Kompas_Plugins_UIComponents_SaveRasterWizard_h
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
 * @brief Class Kompas::Plugins::UIComponents::SaveRasterWizard
 */

#include <QtGui/QWizard>

#include "AbsoluteArea.h"
#include "AbstractRasterModel.h"

namespace Kompas { namespace Plugins { namespace UIComponents {

class SaveRasterThread;
class AreaPage;
class ContentsPage;
class SavePage;
class StatisticsPage;
class MetadataPage;
class DownloadPage;

/**
 * @brief Wizard for saving raster maps
 * @todo @c VERSION-0.2 Final package size
 */
class SaveRasterWizard: public QWizard {
    Q_OBJECT

    friend class AreaPage;
    friend class ContentsPage;
    friend class DownloadPage;
    friend class MetadataPage;
    friend class StatisticsPage;

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

}}}

#endif
