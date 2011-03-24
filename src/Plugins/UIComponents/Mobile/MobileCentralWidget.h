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

    private:
        QHBoxLayout *_layout;

        QPushButton *_mapButton,
            *_settingsButton,
            *_leftButton,
            *_rightButton;

        QAction *_fullscreenAction,
            *_quitAction;

        void positionButtons(const QSize& widgetSize);
};

}}}

#endif