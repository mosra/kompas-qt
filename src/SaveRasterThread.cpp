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

#include "SaveRasterThread.h"

#include <QtCore/QMetaType>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include "MainWindow.h"
#include "PluginManager.h"

using namespace std;
using namespace Kompas::Utility;
using namespace Kompas::Core;

Q_DECLARE_METATYPE(std::string)

namespace Kompas { namespace QtGui {

SaveRasterThread::SaveRasterThread(QObject* parent): QThread(parent), abort(false), destinationModel(0) {
    manager = new QNetworkAccessManager(this);
    connect(this, SIGNAL(download(std::string,Core::Zoom,Core::TileCoords)), SLOT(startDownload(std::string,Core::Zoom,Core::TileCoords)));
    connect(manager, SIGNAL(finished(QNetworkReply*)), SLOT(finishDownload(QNetworkReply*)));
    qRegisterMetaType<std::string>();
}

SaveRasterThread::~SaveRasterThread() {
    /* Schedule thread to abort and wake it up, if it waits for download to finish */
    abort = true;
    condition.wakeOne();

    /* Wait for thread to finish */
    wait();

    /* If the package is not done, finalize it to make it kind of usable */
    if(destinationModel) {
        destinationModel->finalizePackage();
        delete destinationModel;
    }
}

bool SaveRasterThread::initializePackage(const string& model, const string& filename, const TileSize& tileSize, const vector<Zoom>& _zoomLevels, const TileArea& _area, const vector<string>& _layers, const vector<string>& overlays) {
    /* Cleanup previous */
    if(destinationModel) {
        delete destinationModel;
        destinationModel = 0;
        zoomLevels.clear();
        area = TileArea();
        layers.clear();
    }

    /* Get model instance */
    if(!(destinationModel = MainWindow::instance()->pluginManagerStore()->rasterModels()->manager()->instance(model)))
        return false;

    /* Initialize package */
    if(!destinationModel->initializePackage(filename, tileSize, _zoomLevels, _area, _layers, overlays))
        return false;

    /* Save area, zoom levels, merged layers and overlays */
    zoomLevels = _zoomLevels;
    area = _area;
    layers = _layers;
    layers.insert(layers.end(), overlays.begin(), overlays.end());

    return true;
}

void SaveRasterThread::run() {
    if(!destinationModel) return;

    /* Compute tile count for all zoom levels */
    quint64 totalZoom = 0;
    for(vector<Zoom>::const_iterator zit = zoomLevels.begin(); zit != zoomLevels.end(); ++zit) {
        TileArea currentArea = area*pow2(*zit-zoomLevels[0]);
        totalZoom += static_cast<quint64>(currentArea.w)*currentArea.h;
    }
    totalZoom *= layers.size();

    /* Foreach all zoom levels */
    quint64 completedZoom = 0;
    for(vector<Zoom>::const_iterator zit = zoomLevels.begin(); zit != zoomLevels.end(); ++zit) {
        Zoom zoom(*zit);

        /* Compute tile area for this level */
        TileArea currentArea = area*pow2(zoom-zoomLevels[0]);

        quint64 currentAreaSize = static_cast<quint64>(currentArea.w)*currentArea.h;

        /* Foreach all layers */
        int completedLayers = 0;
        for(vector<string>::const_iterator lit = layers.begin(); lit != layers.end(); ++lit) {
            string layer(*lit);

            /* Foreach all rows */
            for(unsigned int row = 0; row != currentArea.h; ++row) {

                /* Foreach all columns */
                for(unsigned int col = 0; col != currentArea.w; ++col) {
                    if(abort) return;

                    TileCoords coords(currentArea.x+col, currentArea.y+row);

                    /* First try to get tile from file */
                    string data = MainWindow::instance()->rasterModelForWrite()()->tileFromPackage(layer, zoom, coords);

                    /* Otherwise download */
                    if(data.empty()) {
                        emit download(layer, zoom, coords);
                        mutex.lock();
                        condition.wait(&mutex);
                        mutex.unlock();

                        data = lastDownloadedData;
                    }

                    if(!destinationModel->tileToPackage(layer, zoom, coords, data)) {
                        emit error();
                        return;
                    }

                    int tilesCompleted = (static_cast<quint64>(row)*currentArea.w+col+1);

                    emit completeChanged(zoom, zit-zoomLevels.begin()+1, layer, completedLayers+1, (completedZoom+currentAreaSize*completedLayers+tilesCompleted)*100/totalZoom, tilesCompleted*100/currentAreaSize);
                }
            }

            ++completedLayers;
        }

        completedZoom += currentAreaSize*layers.size();
    }

    destinationModel->finalizePackage();
    delete destinationModel;
    destinationModel = 0;
    emit completed();
}

void SaveRasterThread::startDownload(const string& layer, Zoom zoom, const TileCoords& coords) {
    manager->get(QNetworkRequest(QUrl(QString::fromStdString(MainWindow::instance()->rasterModelForRead()()->tileUrl(layer, zoom, coords)))));
}

void SaveRasterThread::finishDownload(QNetworkReply* reply) {
    QByteArray data = reply->readAll();
    lastDownloadedData.assign(data.data(), data.size());
    reply->deleteLater();
    condition.wakeOne();
}

}}
