#ifndef Kompas_Plugins_UIComponents_StatusBarUIComponent_h
#define Kompas_Plugins_UICompoments_StatusBarUIComponent_h
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
 * @brief Class Kompas::Plugins::UIComponents::StatusBarUIComponent
 */

#include "AbstractUIComponent.h"

class QStatusBar;
class QLabel;

#include "LatLonCoords.h"

namespace Kompas {

namespace Core {
    class AbstractRasterModel;
}

namespace Plugins { namespace UIComponents {

/**
@brief Status bar for desktop interface

Shows coordinates under mouse cursor, if current map view and raster model
supports it.
 */
class StatusBarUIComponent: public QtGui::AbstractUIComponent {
    Q_OBJECT

    public:
        /** @copydoc QtGui::AbstractUIComponent::AbstractUIComponent */
        StatusBarUIComponent(PluginManager::AbstractPluginManager* manager = 0, const std::string& plugin = "");

        inline QStatusBar* statusBar() const { return _statusBar; }

    private slots:
        void mapViewChanged();
        void rasterModelChanged(const Core::AbstractRasterModel* previous);
        void currentCoordinates(const Core::LatLonCoords& coords);

    private:
        QStatusBar* _statusBar;
        QLabel* _coordinateStatus;
};

}}}

#endif
