#ifndef Kompas_QtGui_RasterOverlayModel_h
#define Kompas_QtGui_RasterOverlayModel_h
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

/** @file
 * @brief Class Kompas::QtGui::RasterOverlayModel
 */

#include <QtCore/QAbstractListModel>
#include <QtCore/QStringList>

namespace Kompas { namespace QtGui {

/**
 * @brief Model for tile overlays
 *
 * Single-column model which displays all available overlayss of given tile model.
 */
class RasterOverlayModel: public QAbstractListModel {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param parent            Parent object
         */
        inline RasterOverlayModel(QObject* parent = 0):
            QAbstractListModel(parent) { reload(); }

        /** @brief Row count */
        inline virtual int rowCount(const QModelIndex& parent = QModelIndex()) const
            { return overlays.count(); }

        /** @brief Data read access */
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

        /**
         * @brief Find overlay
         * @param overlay   Overlay name
         * @return Index of the overlay or invalid index, if the overlay was
         * not found.
         */
        QModelIndex find(const QString& layer);

    public slots:
        /**
         * @brief Reload data
         *
         * Should be called when tile model is changed or overlays are changed.
         */
        void reload();

    private:
        QStringList overlays;
};

}}

#endif
