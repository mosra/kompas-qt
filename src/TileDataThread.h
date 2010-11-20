#ifndef Map2X_QtGui_TileDataThread_h
#define Map2X_QtGui_TileDataThread_h
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
 * @brief Class Map2X::QtGui::TileDataThread
 */

#include <QtCore/QHash>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtGui/QPixmap>

#include "AbstractRasterModel.h"

class QNetworkReply;
class QNetworkAccessManager;

namespace Map2X { namespace QtGui {

/**
 * @brief Thread for getting tile data
 *
 * Getting tile data from local files in done in separated thread, if the data
 * aren't available locally and model has enabled online
 * @todo Passing downloaded tile back to model
 * @todo Cleanup, better removing of finushed jobs (without large foreach)
 * @todo Generalize for all data?
 */
class TileDataThread: public QThread {
    Q_OBJECT

    public:
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
        virtual void run();

    public slots:
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

        struct TileJob {
            QNetworkReply* reply;
            QString layer;
            Core::Zoom zoom;
            Core::TileCoords coords;
            bool running;

            TileJob(): reply(0), running(false) {}
        };

        QMutex mutex;
        QWaitCondition condition;
        bool abort;

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
        void download(TileJob* job);

    private slots:
        void startDownload(TileJob* job);
        void finishDownload(QNetworkReply* reply);
};

}}

#endif
