#ifndef Kompas_QtGui_RasterLayerModel_h
#define Kompas_QtGui_RasterLayerModel_h
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
 * @brief Class Kompas::QtGui::RasterLayerModel
 */

#include <QtCore/QAbstractTableModel>
#include <QtCore/QStringList>

namespace Kompas { namespace QtGui {

/**
 * @brief Model for tile layers
 *
 * Displays all available layers of current tile model.
 */
class RasterLayerModel: public QAbstractTableModel {
    Q_OBJECT

    public:
        /** @brief Columns */
        enum Column {
            Name,       /**< @brief Layer name */
            Translated  /**< @brief Translated layer name */
        };

        /**
         * @brief Constructor
         * @param parent            Parent object
         */
        inline RasterLayerModel(QObject* parent = 0):
            QAbstractTableModel(parent) { reload(); }

        /** @brief Column count */
        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const { return 2; }

        /** @brief Row count */
        inline virtual int rowCount(const QModelIndex& parent = QModelIndex()) const
            { return layers.count(); }

        /** @brief Data read access */
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

        /**
         * @brief Find layer
         * @param layer     Layer name
         * @return Index of the layer or invalid index, if the layer was not
         * found.
         */
        QModelIndex find(const QString& layer);

    public slots:
        /**
         * @brief Reload data
         *
         * Should be called when tile model is changed or layers are changed.
         */
        void reload();

    private:
        struct Layer {
            QString name;
            const std::string* translated;
        };

        QList<Layer> layers;
};

}}

#endif
