#ifndef Kompas_QtGui_TileDataThread_h
#define Kompas_QtGui_TileDataThread_h
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
 * @brief Class Kompas::QtGui::TileDataThread
 */

#include <QtCore/QHash>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtGui/QPixmap>

#include "AbstractRasterModel.h"

class QNetworkReply;
class QNetworkAccessManager;

namespace Kompas { namespace QtGui {

/**
 * @brief Thread for getting tile data
 *
 * Getting tile data from local files in done in separated thread, if the data
 * aren't available locally and model has enabled online
 * @todo @c VERSION-0.2 Passing downloaded tile back to model
 * @todo Generalize for all data?
 */
class TileDataThread: public QThread {
    Q_OBJECT

    public:
        /** @brief Job for tile data */
        struct TileJob {
            QNetworkReply* reply;       /**< @brief Network reply (if the tile is being downloaded) */
            QString layer;              /**< @brief Tile layer */
            Core::Zoom zoom;            /**< @brief Tile zoom */
            Core::TileCoords coords;    /**< @brief Tile coordinates */
            bool running;               /**< @brief Whether tile download is in progress */
            QByteArray downloadedData;  /**< @brief Downloaded data */

            /** @brief Constructor */
            inline TileJob(): reply(0), running(false) {}
        };

        /**
         * @brief Maximum count of simultaenous downloads
         *
         * Default count is 3.
         */
        inline static int maxSimultaenousDownloads() { return _maxSimultaenousDownloads; }

        /**
         * @brief Set maximum count of simultaenous downloads
         * @param count     Count
         *
         * Lowest valid value is 1, highest 10. If the value is out of bounds,
         * nearest possible value will be applied.
         */
        inline static void setMaxSimultaenousDownloads(int count) {
            if(count < 1)
                _maxSimultaenousDownloads = 1;
            else if(count > 10)
                _maxSimultaenousDownloads = 10;
            else
                _maxSimultaenousDownloads = count;
        }

        /**
         * @brief Constructor
         * @param parent        Parent object
         */
        TileDataThread(QObject* parent = 0);

        /**
         * @brief Destructor
         *
         * Aborts all network requests and waits for the thread to finish.
         */
        virtual ~TileDataThread();

        /**
         * @brief Main thread loop
         */
        void run();

        /**
         * @brief Request for tile data
         * @param layer     Tile layer or overlay name
         * @param z         Zoom
         * @param coords    Tile coordinates
         *
         * Searches through local data for tile data, if found, emits tileData()
         * signal. If not found, tries to load it from URL (if online is
         * enabled) and emits tileDataDownloading() signal with "loading" state
         * image and tileData() signal with downloaded tile data. If online is
         * not enabled or tile cannot be downloaded, emits tileDataNotFound()
         * signal with "not found" state image.
         */
        void getTileData(const QString& layer, Core::Zoom z, const Core::TileCoords& coords);

        /**
         * @brief Abort jobs in queue
         * @param layer     Layer to abort. If empty, aborts all jobs.
         */
        void abort(const QString& layer = "");

    signals:
        /**
         * @brief Tile data
         * @param layer     Tile layer or overlay name
         * @param z         Zoom
         * @param coords    Tile coordinates
         * @param data      Tile data
         */
        void tileData(const QString& layer, Core::Zoom z, const Core::TileCoords& coords, const QByteArray& data);

        /**
         * @brief Tile loading
         * @param layer     Tile layer or overlay name
         * @param z         Zoom
         * @param coords    Tile coordinates
         *
         * Emitted when the tile is added to queue.
         */
        void tileLoading(const QString& layer, Core::Zoom z, const Core::TileCoords& coords);

        /**
         * @brief Tile not found
         * @param layer     Tile layer or overlay name
         * @param z         Zoom
         * @param coords    Tile coordinates
         *
         * Emitted when online maps are disabled and tile was not found locally
         * or when downloading tile failed.
         */
        void tileNotFound(const QString& layer, Core::Zoom z, const Core::TileCoords& coords);

    private:
        static int _maxSimultaenousDownloads;

        QMutex mutex;
        QWaitCondition condition;
        bool _abort;

        QNetworkAccessManager* manager;
        QList<TileJob> queue;

    signals:
        /**
         * @brief Download given tile
         *
         * Emitted from getTileData(), connected to private slot
         * startDownload(). The slot is not called directly because
         * QNetworkManager somehow doesn't like sharing QNetworkReply between
         * different threads.
         */
        void download(const Kompas::QtGui::TileDataThread::TileJob& job);

    private slots:
        void startDownload(const Kompas::QtGui::TileDataThread::TileJob job);
        void finishDownload(QNetworkReply* reply);
};

}}

#endif
