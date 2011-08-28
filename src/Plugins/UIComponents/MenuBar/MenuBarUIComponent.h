#ifndef Kompas_Plugins_UIComponents_MenuBarUIComponent_h
#define Kompas_Plugins_UICompoments_MenuBarUIComponent_h
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
 * @brief Class Kompas::Plugins::UIComponents::MenuBarUIComponent
 */

#include "AbstractUIComponent.h"

class QAction;
class QMenu;

namespace Kompas { namespace Plugins { namespace UIComponents {

/**
 * @brief Menu bar for desktop interface
 */
class MenuBarUIComponent: public QtGui::AbstractUIComponent {
    public:
        /** @copydoc QtGui::AbstractUIComponent::AbstractUIComponent */
        MenuBarUIComponent(PluginManager::AbstractPluginManager* manager = 0, const std::string& plugin = "");

        inline QMenuBar* menuBar() const { return _menuBar; }

        void actionAdded(int category, QAction* action);

    private:
        QMenuBar* _menuBar;

        QMenu *_fileMenu,
            *_toolsMenu,
            *_settingsMenu,
            *_helpMenu;

        QAction *_packagesSeparator,
            *_quitSeparator,
            *_quitAction;
};

}}}

#endif
