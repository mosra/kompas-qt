#ifndef Kompas_Plugins_UIComponents_CacheTab_h
#define Kompas_Plugins_UIComponents_CacheTab_h
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
 * @brief Class Kompas::Plugins::UIComponents::CacheTab
 */

#include "AbstractConfigurationWidget.h"

class QLabel;
class QPushButton;
class QProgressBar;
class QSpinBox;
class QComboBox;
class QLineEdit;

namespace Kompas {

namespace QtGui {
    class PluginModel;
}

namespace Plugins { namespace UIComponents {

/**
@brief Cache configuration tab

Cache configuration can be slow, thus all operations are done in separate
thread and the dialog indicates that an operation is in progress.

Only cache dir and cache plugin is stored in configuration, cache size and
block size stores the cache itself. Default values for cache size and block
size are used only when new cache is set up (the plugin name changes or the dir
changes). These values are on cache initialization saved as default (and used
in future as default for new caches).
*/
class CacheTab: public QtGui::AbstractConfigurationWidget {
    Q_OBJECT

    public:
        /** @copydoc QtGui::AbstractConfigurationWidget::AbstractConfigurationWidget */
        CacheTab(QWidget* parent = 0, Qt::WindowFlags f = 0);

    public slots:
        void reset();
        void restoreDefaults();
        void save();

    private slots:
        void resetCacheSize();
        void initialize();
        void selectCacheDir();
        void setSize();
        void optimize();
        void purge();

        void finishBlockingOperation();

    private:
        QtGui::PluginModel *pluginModel;
        QComboBox *plugin;
        QLabel* usageLabel;
        QLineEdit *dir;
        QProgressBar* usage;
        QPushButton *optimizeButton, *purgeButton;
        QSpinBox *size, *blockSize;

        void startBlockingOperation(const QString& description);

        void initializeInternal();
        void setSizeInternal(size_t size);
        void setBlockSizeInternal(size_t size);
        void optimizeInternal();
        void purgeInternal();
};

}}}

#endif
