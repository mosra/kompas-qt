#include "PluginManager/AbstractPluginManager.h"
#include "Utility/utilities.h"

int registerQtStaticPlugins() {
    PLUGIN_IMPORT(DistanceMeterTool)
    PLUGIN_IMPORT(DmsDecimalTool)
    PLUGIN_IMPORT(GraphicsMapView)
    PLUGIN_IMPORT(ConfigurationUIComponent)
    PLUGIN_IMPORT(MenuBarUIComponent)
    PLUGIN_IMPORT(PluginConfigurationUIComponent)
    return 1;
} AUTOMATIC_INITIALIZER(registerQtStaticPlugins)
