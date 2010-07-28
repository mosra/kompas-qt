#ifndef Map2X_QtGui_AbstractConfigurationWidget_h
#define Map2X_QtGui_AbstractConfigurationWidget_h
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
 * @brief Class Map2X::QtGui::AbstractConfigurationWidget
 */

#include <QtGui/QWidget>

namespace Map2X { namespace QtGui {

/** @brief Abstract configuration widget */
class AbstractConfigurationWidget: public QWidget {
    Q_OBJECT

    public:
        /**
         * @brief Constructor
         * @param parent    Parent widget
         * @param f         Window flags
         */
        AbstractConfigurationWidget(QWidget* parent = 0, Qt::WindowFlags f = 0):
            QWidget(parent, f) {}

    public slots:
        virtual void save() = 0;            /**< @brief Save changes to configuration */
        virtual void reset() = 0;           /**< @brief Reset changes */
        virtual void restoreDefaults() = 0; /**< @brief Load default configuration */

    signals:
        void edited(bool = true);           /**< @brief Emitted when the data are changed */
};

}}

#endif
