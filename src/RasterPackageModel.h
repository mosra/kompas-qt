#ifndef Map2X_QtGui_RasterPackageModel_h
#define Map2X_QtGui_RasterPackageModel_h
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
 * @brief Class Map2X::QtGui::RasterPackageModel
 */

#include <QtCore/QAbstractListModel>

namespace Map2X { namespace QtGui {

class RasterPackageModel: public QAbstractTableModel {
    Q_OBJECT

    public:
        enum Column {
            Name,
            Description,
            Packager
        };

        inline RasterPackageModel(QObject* parent = 0): QAbstractTableModel(parent) {}

        inline virtual int columnCount(const QModelIndex& parent = QModelIndex()) const
            { return 3; }
        inline virtual int rowCount(const QModelIndex& parent = QModelIndex()) const
            { return packages.count(); }
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
            QString name,
                    description,
                    packager;
        };

        QList<Package> packages;
};

}}

#endif
