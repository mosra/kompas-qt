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

#include "TileDataThread.h"

#include <QtCore/QMetaType>
#include <QtGui/QPixmap>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

#include "MainWindow.h"

using namespace std;
using namespace Kompas::Core;

Q_DECLARE_METATYPE(Kompas::QtGui::TileDataThread::TileJob)

namespace Kompas { namespace QtGui {

int TileDataThread::_maxSimultaenousDownloads = 3;

TileDataThread::TileDataThread(QObject* parent): QThread(parent), _abort(false) {
    qRegisterMetaType<TileJob>();

    manager = new QNetworkAccessManager(this);
    connect(this, SIGNAL(download(Kompas::QtGui::TileDataThread::TileJob)), this, SLOT(startDownload(Kompas::QtGui::TileDataThread::TileJob)));
    connect(manager, SIGNAL(finished(QNetworkReply*)), SLOT(finishDownload(QNetworkReply*)));
}

TileDataThread::~TileDataThread() {
    mutex.lock();
    _abort = true;
    condition.wakeOne();
    mutex.unlock();

    wait();
}

void TileDataThread::run() {
    forever {

        /* If aborting, stop all network requests and stop thread */
        if(_abort) {
            abort();
            return;
        }

        mutex.lock();

        /* Foreach queue and check whether we can run another job */
        int runningCount = 0;
        for(int i = 0; i != queue.size(); ++i)
            if(queue[i].running) runningCount++;

        /* If we can, find first pending job and remove it from the queue */
        TileJob firstPending;
        if(runningCount < _maxSimultaenousDownloads) {
            for(int i = 0; i != queue.size(); ++i) if(!queue[i].running) {
                firstPending = queue[i];
                queue.removeAt(i);
                runningCount = -1;
                break;
            }
        }

        mutex.unlock();

        /* Job found, proceed */
        if(runningCount == -1) {
            Locker<AbstractRasterModel> rasterModel = MainWindow::instance()->rasterModelForWrite();

            /* Tile is already downloaded, save it to cache and continue to another */
            if(rasterModel() && !firstPending.downloadedData.isEmpty()) {
                rasterModel()->tileToCache(firstPending.layer.toStdString(), firstPending.zoom, firstPending.coords, string(firstPending.downloadedData.data(), firstPending.downloadedData.size()));

            } else {
                /* No model available */
                if(!rasterModel()) {
                    emit tileNotFound(firstPending.layer, firstPending.zoom, firstPending.coords);
                    return;
                }

                /* First try to get the data from package or cache */
                string data = rasterModel()->tileFromPackage(firstPending.layer.toStdString(), firstPending.zoom, firstPending.coords);
                if(data.empty())
                    data = rasterModel()->tileFromCache(firstPending.layer.toStdString(), firstPending.zoom, firstPending.coords);
                bool online = rasterModel()->online();
                rasterModel.unlock();

                /* If found, emit signal with data */
                if(!data.empty()) {

                    /* QByteArray::fromRawData() doesn't copy data under pointer,
                    "change" them to force deep copy */
                    QByteArray b = QByteArray::fromRawData(data.data(), data.size());
                    b[0] = b[0];

                    emit tileData(firstPending.layer, firstPending.zoom, firstPending.coords, b);

                /* Else try to download the item */
                } else {
                    /* Online is not enabled, tile not found */
                    if(!online)
                        emit tileNotFound(firstPending.layer, firstPending.zoom, firstPending.coords);

                    /* Add the item back to queue, request download */
                    else {
                        mutex.lock();
                        firstPending.running = true;
                        queue.append(firstPending);
                        mutex.unlock();

                        emit download(firstPending);
                    }
                }
            }

        /* If nothing to do, wait until next wakeup */
        } else {
            mutex.lock();
            condition.wait(&mutex);
            mutex.unlock();
        }
    }
}

void TileDataThread::startDownload(TileJob job) {
    QMutexLocker locker(&mutex);

    int position = -1;
    for(int i = 0; i != queue.size(); ++i) if(job.coords == queue[i].coords && job.layer == queue[i].layer && job.zoom == queue[i].zoom && job.reply == 0) {
        position = i;
        break;
    }

    /* The job was already aborted, nothing to do */
    if(position == -1) return;

    /* Create request for given tile */
    QString url = QString::fromStdString(MainWindow::instance()->rasterModelForRead()()->tileUrl(job.layer.toStdString(), job.zoom, job.coords));

    queue[position].reply = manager->get(QNetworkRequest(QUrl(url)));
}

void TileDataThread::getTileData(const QString& layer, Core::Zoom z, const Core::TileCoords& coords) {
    emit tileLoading(layer, z, coords);

    QMutexLocker locker(&mutex);

    /* If the job is already in the queue, don't add ít again */
    foreach(const TileJob& job, queue)
        if(job.coords == coords && job.layer == layer && job.zoom == z) return;

    /* Add tile request to the queue */
    TileJob dl;
    dl.zoom = z;
    dl.layer = layer;
    dl.coords = coords;

    queue.append(dl);

    /* If the thread is not running, start it, otherwise wake up */
    if(!isRunning()) start();
    else condition.wakeOne();
}

void TileDataThread::abort(const QString& layer) {
    mutex.lock();
    for(int i = queue.size()-1; i >= 0; --i) {
        if(!layer.isEmpty() && queue[i].layer != layer)
            continue;

        if(queue[i].reply) {
            queue[i].reply->abort();
            queue[i].reply->deleteLater();
        }

        queue.removeAt(i);
    }
    mutex.unlock();
}

void TileDataThread::finishDownload(QNetworkReply* reply) {
    /* Triggerred from abort() */
    if(reply->error() == QNetworkReply::OperationCanceledError) return;

    /* Find the reply in queue, save the data there */
    TileJob dl;

    mutex.lock();
    QByteArray data;
    for(int i = 0; i != queue.size(); ++i) if(queue[i].reply == reply) {
        dl = queue[i];

        data = reply->readAll();
        if(data.isEmpty()) {
            queue.removeAt(i);
        } else {
            queue[i].downloadedData = data;
            queue[i].running = false;
            queue[i].reply = 0;
        }
        break;
    }
    mutex.unlock();

    /* Download failed */
    if(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 200) {
        emit tileNotFound(dl.layer, dl.zoom, dl.coords);

    /* Download success */
    } else emit tileData(dl.layer, dl.zoom, dl.coords, data);

    reply->deleteLater();
    condition.wakeOne();
}

}}
