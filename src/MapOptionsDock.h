#ifndef Map2X_QtGui_MapOptionsDock_h
#define Map2X_QtGui_MapOptionsDock_h
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
 * @brief Class Map2X::QtGui::MapOptionsDock
 */

#include <QtCore/QStringList>
#include <QtCore/QBitArray>
#include <QtGui/QWidget>
#include <QtGui/QAbstractProxyModel>

class QListView;
class QComboBox;

namespace Map2X { namespace QtGui {

class MainWindow;
class PluginModel;
class RasterOverlayModel;
class RasterLayerModel;
class AbstractMapView;

/** @brief Dock widget with map options */
class MapOptionsDock: public QWidget {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param _mainWindow       Main window instance
         * @param parent            Parent widget
         * @param f                 Window flags
         */
        MapOptionsDock(MainWindow* _mainWindow, QWidget* parent = 0, Qt::WindowFlags f = 0);

    protected:
        class EditableRasterOverlayModel;

    private:
        MainWindow* mainWindow;

        QComboBox *rasterModels,
            *rasterLayers;
        QListView* rasterOverlays;

        PluginModel* rasterModelsModel;
        RasterLayerModel* rasterLayerModel;
        RasterOverlayModel* rasterOverlayModel;

    private slots:
        void setRasterModel(int number);

        /** @brief Update comboboxes to actually used layers etc. */
        void setActualData();
};

class MapOptionsDock::EditableRasterOverlayModel: public QAbstractProxyModel {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param _mapView          Map view which displays map from given tile
         *      model
         * @param parent            Parent object
         */
        inline EditableRasterOverlayModel(AbstractMapView** _mapView, QObject* parent = 0):
            QAbstractProxyModel(parent), mapView(_mapView) {}

        inline virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const
            { return createIndex(row, column); }
        inline virtual QModelIndex parent(const QModelIndex& child) const
            { return QModelIndex(); }
        inline virtual int columnCount(const QModelIndex& parent = QModelIndex()) const
            { return 1; }
        inline virtual int rowCount(const QModelIndex& parent = QModelIndex()) const
            { return sourceModel()->rowCount(); }

        virtual void setSourceModel(QAbstractItemModel* sourceModel);
        virtual QModelIndex mapFromSource(const QModelIndex& sourceIndex) const;
        virtual QModelIndex mapToSource(const QModelIndex& proxyIndex) const;
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
        virtual Qt::ItemFlags flags(const QModelIndex& index) const;
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
        AbstractMapView** mapView;

        QBitArray loaded;
};

}}

#endif
