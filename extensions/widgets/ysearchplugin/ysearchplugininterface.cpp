#include "ysearchplugininterface.h"
#include <QtPlugin>
#include <pluginloader.h>
#include "ysearchplugin.h"

YSearchPluginInterface::YSearchPluginInterface(QObject * parent)
{

}

PlexyDesk::BasePlugin* YSearchPluginInterface::instance()
{
    return new YSearchPlugin(this);
}

Q_EXPORT_PLUGIN2(ysearchplugininterface,YSearchPluginInterface);
