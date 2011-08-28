#ifndef Kompas_Plugins_UIComponents_EditableRasterOverlayModel_h
#define Kompas_Plugins_UIComponents_EditableRasterOverlayModel_h
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
 * @brief Class Kompas::Plugins::UIComponents::EditableRasterOverlayModel
 */

#include <QtCore/QStringList>
#include <QtCore/QBitArray>
#include <QtGui/QAbstractProxyModel>

namespace Kompas { namespace Plugins { namespace UIComponents {

/**
 * @brief Editable raster overlay model
 *
 * Used exclusively in MapOptionsDock, allows selecting overlays for displaying.
 */
class EditableRasterOverlayModel: public QAbstractProxyModel {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param parent            Parent object
         */
        inline EditableRasterOverlayModel(QObject* parent = 0):
            QAbstractProxyModel(parent) {}

        /** @brief Index creation */
        inline QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const
            { return createIndex(row, column); }

        /** @brief Parent index of given index */
        inline QModelIndex parent(const QModelIndex& child) const
            { return QModelIndex(); }

        /** @brief Column count */
        inline int columnCount(const QModelIndex& parent = QModelIndex()) const
            { return sourceModel()->columnCount(); }

        /** @brief Row count */
        inline int rowCount(const QModelIndex& parent = QModelIndex()) const
            { return sourceModel()->rowCount(); }

        /** @brief Set source model */
        void setSourceModel(QAbstractItemModel* sourceModel);

        /** @brief Map index from source model */
        QModelIndex mapFromSource(const QModelIndex& sourceIndex) const {
            return index(sourceIndex.row(), sourceIndex.column());
        }

        /** @brief Map index to source model */
        QModelIndex mapToSource(const QModelIndex& proxyIndex) const {
            return sourceModel()->index(proxyIndex.row(), proxyIndex.column());
        }

        /** @brief Data read access */
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

        /** @brief Item flags */
        Qt::ItemFlags flags(const QModelIndex& index) const;

        /** @brief Data write access */
        bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

    public slots:
        /**
         * @brief Reload data
         *
         * Should be called when tile model is changed, map view is changed or
         * overlays are changed.
         */
        void reload();

        /**
         * @brief Reload currently active overlays
         * @param activeOverlays    Currently active overlays
         */
        void reload(const QStringList& activeOverlays);

    private:
        QBitArray loaded;
};

}}}

#endif
