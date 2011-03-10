#ifndef Kompas_Plugins_UIComponents_MapOptionsUIComponent_h
#define Kompas_Plugins_UIComponents_MapOptionsUIComponent_h
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
 * @brief Class Kompas::Plugins::UIComponents::MapOptionsUIComponent
 */

#include "AbstractUIComponent.h"

namespace Kompas { namespace Plugins { namespace UIComponents {

/** @brief Map options dock for desktop interface */
class MapOptionsUIComponent: public QtGui::AbstractUIComponent {
    public:
        /** @copydoc QtGui::AbstractUIComponent::AbstractUIComponent */
        MapOptionsUIComponent(PluginManager::AbstractPluginManager* manager = 0, const std::string& plugin = "");

        inline virtual QDockWidget* dockWidget(Qt::DockWidgetArea* area) const {
            *area = Qt::RightDockWidgetArea;
            return _dockWidget;
        }

    private:
        QDockWidget* _dockWidget;
};

}}}

#endif
