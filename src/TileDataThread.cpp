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

#include <QtCore/QMetaType>
#include <QtGui/QPixmap>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

#include "MainWindow.h"

using namespace std;
using namespace Map2X::Core;

Q_DECLARE_METATYPE(Map2X::QtGui::TileDataThread::TileJob)

namespace Map2X { namespace QtGui {

int TileDataThread::_maxSimultaenousDownloads = 3;

TileDataThread::TileDataThread(QObject* parent): QThread(parent), _abort(false) {
    qRegisterMetaType<TileJob>();

    manager = new QNetworkAccessManager(this);
    connect(this, SIGNAL(download(Map2X::QtGui::TileDataThread::TileJob)), this, SLOT(startDownload(Map2X::QtGui::TileDataThread::TileJob)));
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

            AbstractRasterModel* rasterModel = MainWindow::instance()->lockRasterModelForWrite();

            /* No model available */
            if(!rasterModel) {
                MainWindow::instance()->unlockRasterModel();
                emit tileNotFound(firstPending.layer, firstPending.zoom, firstPending.coords);
                return;
            }

            /* First try to get the data locally */
            string data = rasterModel->tileFromPackage(firstPending.layer.toStdString(), firstPending.zoom, firstPending.coords);
            /** @todo @c VERSION-0.2 Get data also from cache */
            bool online = rasterModel->online();

            MainWindow::instance()->unlockRasterModel();

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
    const AbstractRasterModel* model = MainWindow::instance()->lockRasterModelForRead();
    QString url = QString::fromStdString(model->tileUrl(job.layer.toStdString(), job.zoom, job.coords));
    MainWindow::instance()->unlockRasterModel();

    queue[position].reply = manager->get(QNetworkRequest(QUrl(url)));
}

void TileDataThread::getTileData(const QString& layer, Core::Zoom z, const Core::TileCoords& coords) {
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

    /* Find the reply in queue */
    TileJob dl;

    mutex.lock();
    for(int i = 0; i != queue.size(); ++i) if(queue[i].reply == reply) {
        dl = queue[i];
        queue.removeAt(i);
        break;
    }
    mutex.unlock();

    /* Download failed */
    if(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 200) {
        emit tileNotFound(dl.layer, dl.zoom, dl.coords);

    /* Download success */
    } else emit tileData(dl.layer, dl.zoom, dl.coords, reply->readAll());

    reply->deleteLater();
    condition.wakeOne();
}

}}
