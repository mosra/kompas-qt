#ifndef Kompas_Plugins_UIComponents_EditableRasterPackageModel_h
#define Kompas_Plugins_UIComponents_EditableRasterPackageModel_h
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
 * @brief Class Kompas::Plugins::UIComponents::EditableRasterPackageModel
 */

#include <QtGui/QAbstractProxyModel>

namespace Kompas { namespace Plugins { namespace UIComponents {

/**
 * @brief Editable raster package model
 *
 * Used exclusively in MapOptionsDock, allows viewing currently opened packages
 * and, if current model supports it, enabling/disabling online maps.
 */
class EditableRasterPackageModel: public QAbstractProxyModel {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param parent            Parent object
         */
        inline EditableRasterPackageModel(QObject* parent = 0):
            QAbstractProxyModel(parent), online(NotSupported) {}

        /** @brief Index creation */
        inline virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const
            { return createIndex(row, column); }

        /** @brief Parent index of given index */
        inline virtual QModelIndex parent(const QModelIndex& child) const
            { return QModelIndex(); }

        /** @brief Column count */
        inline virtual int columnCount(const QModelIndex& parent = QModelIndex()) const
            { return sourceModel()->columnCount(); }

        /** @brief Row count */
        inline virtual int rowCount(const QModelIndex& parent = QModelIndex()) const
            { return sourceModel()->rowCount() + (online == NotSupported ? 0 : 1); }

        /** @brief Set source model */
        virtual void setSourceModel(QAbstractItemModel* sourceModel);

        /** @brief Map index from source model */
        virtual QModelIndex mapFromSource(const QModelIndex& sourceIndex) const {
            return index(sourceIndex.row(), sourceIndex.column());
        }

        /** @brief Map index to source model */
        virtual QModelIndex mapToSource(const QModelIndex& proxyIndex) const {
            return sourceModel()->index(proxyIndex.row(), proxyIndex.column());
        }

        /** @brief Data read access */
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

        /** @brief Item flags */
        virtual Qt::ItemFlags flags(const QModelIndex& index) const;

        /** @brief Data write access */
        virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

    public slots:
        /**
         * @brief Reload data
         *
         * Should be called when tile model is changed, map view is changed or
         * overlays are changed.
         */
        void reload();

    private:
        enum OnlineState {
            NotSupported,
            Disabled,
            Enabled
        };

        OnlineState online;
        QString rasterModelName;
};

}}}

#endif
