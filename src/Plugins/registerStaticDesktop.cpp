#include "PluginManager/AbstractPluginManager.h"
#include "Utility/utilities.h"

int registerQtDesktopStaticPlugins() {
    PLUGIN_IMPORT(GraphicsMapView)

    PLUGIN_IMPORT(AboutUIComponent)
    PLUGIN_IMPORT(ConfigurationUIComponent)
    PLUGIN_IMPORT(MenuBarUIComponent)
    PLUGIN_IMPORT(PluginConfigurationUIComponent)
    PLUGIN_IMPORT(DesktopUIComponent)
    PLUGIN_IMPORT(MapOptionsUIComponent)
    PLUGIN_IMPORT(OpenRasterUIComponent)
    PLUGIN_IMPORT(SessionManagementUIComponent)
    PLUGIN_IMPORT(StatusBarUIComponent)
    return 1;
} AUTOMATIC_INITIALIZER(registerQtDesktopStaticPlugins)
