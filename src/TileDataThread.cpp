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

#include "TileDataThread.h"

#include <QtGui/QPixmap>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

using namespace std;
using namespace Map2X::Core;

namespace Map2X { namespace QtGui {

TileDataThread::TileDataThread(AbstractTileModel** _model, QMutex* _modelMutex, QObject* parent): QThread(parent), model(_model), modelMutex(_modelMutex), abort(false) {
    manager = new QNetworkAccessManager;
    connect(this, SIGNAL(download(TileJob*)), this, SLOT(startDownload(TileJob*)));
    connect(manager, SIGNAL(finished(QNetworkReply*)), SLOT(finishDownload(QNetworkReply*)));
}

TileDataThread::~TileDataThread() {
    mutex.lock();
    abort = true;
    condition.wakeOne();
    mutex.unlock();

    wait();
}

void TileDataThread::run() {
    forever {
        mutex.lock();

        /* If aborting, stop all network requests and stop thread */
        if(abort) {
            foreach(TileJob tile, queue) {
                if(tile.reply) tile.reply->abort();
                delete tile.reply;
            }

            mutex.unlock();
            return;
        }

        /* Foreach queue and find pending jobs */
        int running = 0;
        TileJob* firstPending = 0;
        for(int i = 0; i != queue.size(); ++i) {
            if(queue[i].running) running++;
            else if(!firstPending) firstPending = &queue[i];
        }

        mutex.unlock();

        /* If any job is waiting, proceed */
        if(running < maxSimultaenousDownloads && firstPending) {

            /* Make copy of the tile data to avoid unnecessary mutex locks */
            mutex.lock();
            Zoom zoom = firstPending->zoom;
            TileCoords coords = firstPending->coords;
            QString layer = firstPending->layer;
            mutex.unlock();

            /* No model available */
            modelMutex->lock();
            if(!*model) {
                modelMutex->unlock();
                for(int i = 0; i != queue.size(); ++i) {
                    if(firstPending == &queue[i]) {
                        queue.removeAt(i);
                        break;
                    }
                }
                emit tileNotFound(layer, zoom, coords);
                return;
            }

            /* First try to get the data locally */
            string data = (*model)->tileData(layer.toStdString(), zoom, coords);
            modelMutex->unlock();

            /* If found, emit signal with data and remove from queue */
            if(!data.empty()) {
                mutex.lock();
                for(int i = 0; i != queue.size(); ++i) {
                    if(firstPending == &queue[i]) {
                        queue.removeAt(i);
                        break;
                    }
                }
                mutex.unlock();

                QPixmap pixmap;
                pixmap.loadFromData(data.c_str());
                emit tileData(layer, zoom, coords, pixmap);

            /* Else try to download the item */
            } else {
                modelMutex->lock();

                /* Online is not enabled, tile not found */
                if(!(*model)->online()) {
                    modelMutex->unlock();

                    mutex.lock();
                    for(int i = 0; i != queue.size(); ++i) {
                        if(firstPending == &queue[i]) {
                            queue.removeAt(i);
                            break;
                        }
                    }
                    mutex.unlock();
                    emit tileNotFound(layer, zoom, coords);

                /* Request item download */
                } else {
                    modelMutex->unlock();
                    mutex.lock();
                    firstPending->running = true;
                    mutex.unlock();
                    emit download(firstPending);
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

void TileDataThread::startDownload(TileJob* job) {
    mutex.lock();
    modelMutex->lock();

    /* Create request for given tile */
    job->reply = manager->get(QNetworkRequest(QUrl(
        QString::fromStdString((*model)->tileUrl(job->layer.toStdString(), job->zoom, job->coords)))));

    modelMutex->unlock();
    mutex.unlock();
}

void TileDataThread::getTileData(const QString& layer, Zoom z, const TileCoords& coords) {
    emit tileLoading(layer, z, coords);

    /* Add tile request to the queue */
    TileJob dl;
    dl.zoom = z;
    dl.layer = layer;
    dl.coords = coords;

    mutex.lock();
    queue.append(dl);
    mutex.unlock();

    /* If the thread is not running, start it, otherwise wake up */
    if(!isRunning()) start();
    else condition.wakeOne();
}

void TileDataThread::finishDownload(QNetworkReply* reply) {
    QMutexLocker locker(&mutex);

    /* Find the reply in queue */
    TileJob dl;
    for(int i = 0; i != queue.size(); ++i) {
        if(queue[i].reply == reply) {
            dl = queue[i];
            queue.removeAt(i);
            break;
        }
    }

    /* Download failed */
    if(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 200) {
        emit tileNotFound(dl.layer, dl.zoom, dl.coords);

    /* Download success */
    } else {
        QPixmap pixmap;
        pixmap.loadFromData(reply->readAll());
        emit tileData(dl.layer, dl.zoom, dl.coords, pixmap);
    }

    reply->deleteLater();
    condition.wakeOne();
}

}}
