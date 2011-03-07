#include "PluginManager/AbstractPluginManager.h"
#include "Utility/utilities.h"

int registerQtStaticPlugins() {
    PLUGIN_IMPORT(GraphicsMapView)

    PLUGIN_IMPORT(AboutUIComponent)
    PLUGIN_IMPORT(ConfigurationUIComponent)
    PLUGIN_IMPORT(MenuBarUIComponent)
    PLUGIN_IMPORT(PluginConfigurationUIComponent)
    PLUGIN_IMPORT(DistanceMeterUIComponent)
    PLUGIN_IMPORT(DmsDecimalConverterUIComponent)
    return 1;
} AUTOMATIC_INITIALIZER(registerQtStaticPlugins)
