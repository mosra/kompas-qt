#ifndef Map2X_QtGui_RasterZoomModel_h
#define Map2X_QtGui_RasterZoomModel_h
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
 * @brief Class Map2X::QtGui::RasterZoomModel
 */

#include <QtCore/QAbstractListModel>
#include <QtCore/QStringList>

#include "AbstractRasterModel.h"

namespace Map2X { namespace QtGui {

/**
 * @brief Model for tile zoom levels
 *
 * Single-column model which displays all available zoom levels of given tile model.
 */
class RasterZoomModel: public QAbstractListModel {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param parent            Parent object
         */
        inline RasterZoomModel(QObject* parent = 0):
            QAbstractListModel(parent) { reload(); }

        /** @brief Row count */
        inline virtual int rowCount(const QModelIndex& parent = QModelIndex()) const
            { return z.count(); }

        /** @brief Data read access */
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    public slots:
        /**
         * @brief Reload data
         *
         * Should be called when tile model is changed or zoom levels are changed.
         */
        void reload();

    private:
        QList<Core::Zoom> z;
};

}}

#endif
