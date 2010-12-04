#ifndef Kompas_QtGui_SaveRasterThread_h
#define Kompas_QtGui_SaveRasterThread_h
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

#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtNetwork/QNetworkAccessManager>

#include "AbstractRasterModel.h"
#include "MainWindow.h"

namespace Kompas { namespace QtGui {

class SaveRasterThread: public QThread {
    Q_OBJECT

    public:
        SaveRasterThread(QObject* parent = 0);

        virtual ~SaveRasterThread();

        bool initializePackage(const std::string& model, const std::string& filename, const Core::TileSize& tileSize, const std::vector<Core::Zoom>& _zoomLevels, const Core::TileArea& _area, const std::vector<std::string>& _layers, const std::vector<std::string>& overlays);

        inline bool setPackageAttribute(Core::AbstractRasterModel::PackageAttribute type, const std::string& data) {
            if(!destinationModel) return false;
            return destinationModel->setPackageAttribute(type, data);
        }

        void run();

    signals:
        void completeChanged(Core::Zoom currentZoom, int currentZoomNumber, const std::string& currentLayer, int currentLayerNumber, int totalCompleted, int currentZoomLayerCompleted);

        void error();
        void completed();

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

}}

#endif
