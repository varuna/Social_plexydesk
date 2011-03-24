#include "twitterplugininterface.h"
#include <QtPlugin>
#include <pluginloader.h>
#include "twitterplugin.h"

TwitterPluginInterface::TwitterPluginInterface(QObject * parent)
{

}

PlexyDesk::BasePlugin* TwitterPluginInterface::instance()
{
    return new TwitterPlugin(this);
}

Q_EXPORT_PLUGIN2(twitterplugininterface,TwitterPluginInterface);
