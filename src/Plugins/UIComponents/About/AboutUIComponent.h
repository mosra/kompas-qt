#ifndef Kompas_Plugins_UIComponents_AboutUIComponent_h
#define Kompas_Plugins_UIComponents_AboutUIComponent_h
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
 * @brief Class Kompas::Plugins::UIComponents::AboutUIComponent
 */

#include "AbstractUIComponent.h"

class QAction;

namespace Kompas { namespace Plugins { namespace UIComponents {

/** @brief About Kompas and About Qt dialogs */
class AboutUIComponent: public QtGui::AbstractUIComponent {
    Q_OBJECT

    public:
        /** @copydoc QtGui::AbstractUIComponent::AbstractUIComponent */
        AboutUIComponent(PluginManager::AbstractPluginManager* manager = 0, const std::string& plugin = "");

        inline virtual const QList<QAction*>* actions(ActionCategory category) const {
            if(category == QtGui::AbstractUIComponent::Help)
                return &_actions;
            return 0;
        }

    private slots:
        void aboutDialog();

    private:
        QList<QAction*> _actions;
};

}}}

#endif
