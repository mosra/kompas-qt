#ifndef Kompas_Plugins_UIComponents_SaveRasterThread_h
#define Kompas_Plugins_UIComponents_SaveRasterThread_h
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
 * @brief Class Kompas::Plugins::UIComponents::SaveRasterThread
 */

#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtNetwork/QNetworkAccessManager>

#include "AbstractRasterModel.h"
#include "MainWindow.h"

namespace Kompas { namespace Plugins { namespace UIComponents {

/** @brief Thread for saving raster package */
class SaveRasterThread: public QThread {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param parent    Parent object
         */
        SaveRasterThread(QObject* parent = 0);

        /** @brief Destructor */
        virtual ~SaveRasterThread();

        /** @copydoc Core::AbstractRasterModel::initializePackage()
         * @param model     Model plugin name
         */
        bool initializePackage(const std::string& model, const std::string& filename, const Core::TileSize& tileSize, const std::vector<Core::Zoom>& zoomLevels, const Core::TileArea& area, const std::vector<std::string>& layers, const std::vector<std::string>& overlays);

        /** @copydoc Core::AbstractRasterModel::setPackageAttribute() */
        inline bool setPackageAttribute(Core::AbstractRasterModel::PackageAttribute type, const std::string& data) {
            if(!destinationModel) return false;
            return destinationModel->setPackageAttribute(type, data);
        }

        /** @brief Run the thread */
        void run();

    signals:
        /**
         * @brief Completion state changed
         * @param currentZoom           Current zoom level
         * @param currentZoomNumber     Relative to total zoom level count
         * @param currentLayer          Current layer
         * @param currentLayerNumber    Relative to total layer count
         * @param totalCompleted        Percent completed total
         * @param currentZoomLayerCompleted Percent completed for current zoom
         *      and layer pair
         */
        void completeChanged(Core::Zoom currentZoom, int currentZoomNumber, const std::string& currentLayer, int currentLayerNumber, int totalCompleted, int currentZoomLayerCompleted);

        /** @brief Error occured */
        void error();

        /** @brief Saving completed */
        void completed();

        /**
         * @brief Internal download signal
         *
         * Connected to startDownload().
         */
        void download(const std::string& layer, Core::Zoom zoom, const Core::TileCoords& coords);

    private slots:
        void startDownload(const std::string& layer, Core::Zoom zoom, const Core::TileCoords& coords);
        void finishDownload(QNetworkReply* reply);

    private:
        bool abort;

        QNetworkAccessManager* manager;
        QMutex mutex;
        QWaitCondition condition;
        std::string lastDownloadedData;

        Core::AbstractRasterModel* destinationModel;

        std::vector<Core::Zoom> zoomLevels;
        Core::TileArea area;
        std::vector<std::string> layers;
};

}}}

#endif
