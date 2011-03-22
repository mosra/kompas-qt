#include "PluginManager/AbstractPluginManager.h"
#include "Utility/utilities.h"

int registerQtMobileStaticPlugins() {
    PLUGIN_IMPORT(GraphicsMapView)

    PLUGIN_IMPORT(ConfigurationUIComponent)
    PLUGIN_IMPORT(MobileUIComponent)
    PLUGIN_IMPORT(PluginConfigurationUIComponent)
    return 1;
} AUTOMATIC_INITIALIZER(registerQtMobileStaticPlugins)
