#ifndef Kompas_QtGui_RasterPackageModel_h
#define Kompas_QtGui_RasterPackageModel_h
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
 * @brief Class Kompas::QtGui::RasterPackageModel
 */

#include <QtCore/QAbstractListModel>

namespace Kompas { namespace QtGui {

/**
 * @brief Model for raster packages
 */
class RasterPackageModel: public QAbstractTableModel {
    Q_OBJECT

    public:
        /** @brief Columns */
        enum Column {
            Name,           /**< @brief Package name */
            Filename,       /**< @brief Package filename */
            Description,    /**< @brief Package description */
            Packager        /**< @brief Packager name */
        };

        /**
         * @brief Constructor
         * @param parent            Parent object
         */
        inline RasterPackageModel(QObject* parent = 0): QAbstractTableModel(parent) {}

        /** @brief Column count */
        inline virtual int columnCount(const QModelIndex& parent = QModelIndex()) const
            { return 4; }

        /** @brief Row count */
        inline virtual int rowCount(const QModelIndex& parent = QModelIndex()) const
            { return packages.count(); }

        /** @brief Data read access */
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    public slots:
        /**
         * @brief Reload data
         *
         * Should be called when tile model is changed or packages are changed.
         */
        void reload();

    private:
        struct Package {
            QString filename,
                    name,
                    description,
                    packager;
        };

        QList<Package> packages;
};

}}

#endif
