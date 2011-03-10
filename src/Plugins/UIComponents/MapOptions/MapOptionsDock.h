#ifndef Kompas_Plugins_UIComponents_MapOptionsDock_h
#define Kompas_Plugins_UIComponents_MapOptionsDock_h
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

#include <QtGui/QWidget>

class QListView;
class QComboBox;

namespace Kompas { namespace Plugins { namespace UIComponents {

class MovingWidget;
class ZoomSlider;
class EditableRasterOverlayModel;

/** @brief Dock widget with map options */
class MapOptionsDock: public QWidget {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param parent            Parent widget
         * @param f                 Window flags
         */
        MapOptionsDock(QWidget* parent = 0, Qt::WindowFlags f = 0);

    private:
        QComboBox *mapView,
            *rasterLayers;
        QListView *rasterPackages,
            *rasterOverlays;
        MovingWidget* movingWidget;
        ZoomSlider* zoomSlider;

        EditableRasterOverlayModel* rasterOverlayModel;

    private slots:
        void setMapView(int id);

        void setActualLayer(const QString& layer);

        void setActualLayer(int layer);

        /** @brief Connect new map view to this widget */
        void connectMapView();
};

}}}

#endif
