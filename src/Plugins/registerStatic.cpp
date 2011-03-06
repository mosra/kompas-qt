#include "PluginManager/AbstractPluginManager.h"
#include "Utility/utilities.h"

int registerQtStaticPlugins() {
    PLUGIN_IMPORT(DistanceMeterTool)
    PLUGIN_IMPORT(DmsDecimalTool)
    PLUGIN_IMPORT(GraphicsMapView)
    PLUGIN_IMPORT(MenuBarUIComponent)
    return 1;
} AUTOMATIC_INITIALIZER(registerQtStaticPlugins)
