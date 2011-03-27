#ifndef Kompas_Plugins_UIComponents_MobileCentralWidget_h
#define Kompas_Plugins_UIComponents_MobileCentralWidget_h
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
 * @brief Class Kompas::Plugins::UIComponents::MobileCentralWidget
 */

#include <QtGui/QWidget>
#include <QtGui/QResizeEvent>

class QStackedWidget;
class QHBoxLayout;
class QPushButton;
class QAction;

namespace Kompas { namespace Plugins { namespace UIComponents {

/**
 * @brief Central widget for mobile version
 */
class MobileCentralWidget: public QWidget {
    Q_OBJECT

    public:
        enum Controls {
            Default,
            Map,
            Menu
        };

        enum MapControlWidget {
            Packages,
            Online,
            Layers
        };

        enum MenuControlWidget {
            Configuration,
            Plugins
        };

        /** @brief Constructor */
        MobileCentralWidget(QWidget* parent = 0);

        /**
         * @brief Resize event
         *
         * Repositions corner buttons on widget resize.
         */
        inline virtual void resizeEvent(QResizeEvent* event) {
            positionButtons(event->size());
        }

    private slots:
        void mapViewChanged();
        void toggleFullscreen();

        void setControlsVisible(Controls controls);
        inline void setDefaultControlsVisible() { setControlsVisible(Default); }
        inline void setMapControlsVisible() { setControlsVisible(Map); }
        inline void setSettingsControlVisible() { setControlsVisible(Menu); }

        void setMapControlWidgetVisible(MapControlWidget widget);
        inline void setMapPackagesControlWidgetVisible() { setMapControlWidgetVisible(Packages); }
        inline void setMapOnlineControlWidgetVisible() { setMapControlWidgetVisible(Online); }
        inline void setMapLayersControlWidgetVisible() { setMapControlWidgetVisible(Layers); }

        void setMenuControlWidgetVisible(MenuControlWidget widget);
        inline void setMenuConfigurationControlWidgetVisible() { setMenuControlWidgetVisible(Configuration); }
        inline void setMenuPluginsControlWidgetVisible() { setMenuControlWidgetVisible(Plugins); }

    private:
        QStackedWidget *_topLeftCorner,
            *_topRightCorner,
            *_bottomLeftCorner,
            *_bottomRightCorner,
            *_mapControlWidget,
            *_menuControlWidget;

        QHBoxLayout *_layout;

        QPushButton *_mapButton,
            *_menuButton,
            *_leftButton,
            *_rightButton,

            *_onlineButton,
            *_backButton,
            *_layersButton,
            *_packagesButton,

            *_quitButton,
            *_configurationButton,
            *_pluginsButton;

        void positionButtons(const QSize& widgetSize);
};

}}}

#endif