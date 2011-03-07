#ifndef Kompas_Plugins_UIComponents_CurrentCelestialBodyPluginModel_h
#define Kompas_Plugins_UIComponents_CurrentCelestialBodyPluginModel_h
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
 * @brief Class Kompas::Plugins::UIComponents::CurrentCelestialBodyPluginModel
 */

#include <QtGui/QAbstractProxyModel>

namespace Kompas {

namespace Core {
    class AbstractRasterModel;
}

namespace QtGui {
    class PluginModel;
}

namespace Plugins { namespace UIComponents {

/**
 * @brief Proxy for plugin model which marks currently active celestial body
 */
class CurrentCelestialBodyPluginModel: public QAbstractProxyModel {
    Q_OBJECT

    private:
        QAbstractProxyModel::setSourceModel;

    public:
        /**
         * @brief Constructor
         * @param parent            Parent object
         */
        CurrentCelestialBodyPluginModel(QObject* parent = 0);

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
            { return sourceModel()->rowCount(); }

        /** @brief Set source model */
        virtual void setSourceModel(QtGui::PluginModel* sourceModel);

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

    private slots:
        void changeCurrent(const Core::AbstractRasterModel* previous);

    private:
        QtGui::PluginModel* pluginModel;
};

}}}

#endif
