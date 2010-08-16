#ifndef Map2X_QtGui_TileOverlayModel_h
#define Map2X_QtGui_TileOverlayModel_h
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
 * @brief Class Map2X::QtGui::TileOverlayModel
 */

#include <QtCore/QAbstractTableModel>
#include <QtCore/QStringList>
#include <QtCore/QBitArray>

namespace Map2X {

namespace Core {
    class AbstractTileModel;
}

namespace QtGui {

class AbstractMapView;
class MainWindow;

/**
 * @brief Model for tile overlays
 *
 * Provides editable model which allows adding or removing overlays from tile
 * model.
 */
class TileOverlayModel: public QAbstractListModel {
    Q_OBJECT

    public:
        /** @brief Flags */
        enum Flags {
            LoadedOnly      = 0x04  /**< @brief Display only loaded overlays */
        };

        /**
         * @brief Constructor
         * @param _tileModel        Tile model
         * @param _mapView          Map view which displays map from given tile
         *      model
         * @param flags             Flags
         * @param parent            Parent object
         */
        TileOverlayModel(Core::AbstractTileModel** _tileModel, AbstractMapView** _mapView, int flags = 0, QObject* parent = 0);

        inline virtual int rowCount(const QModelIndex& parent = QModelIndex()) const
            { return overlays.count(); }
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    public slots:
        /**
         * @brief Reload data
         *
         * Should be called when tile model is changed, map view is changed or
         * overlays are changed.
         */
        void reload();

    private:
        Core::AbstractTileModel** tileModel;
        AbstractMapView** mapView;
        int _flags;

        QStringList overlays;
        QBitArray loaded;
};

}}

#endif
