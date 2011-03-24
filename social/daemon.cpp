#include "daemon.h"
#include <QtDBus>
#include <QDBusConnection>
#include <pluginloader.h>
#include <QDebug>
#include <QStringList>

namespace PlexyDesk
{
    SocialDaemon::SocialDaemon(QApplication *parent) : QDBusAbstractAdaptor(parent)
    {
        QDBusConnection::sessionBus().registerService("org.plexydesk.social");
    }

    SocialPlugin* SocialDaemon::getPluginInstance(const QString &pluginName)
    {
        QObject *pluginInstance = (PluginLoader::getInstance()->instance(pluginName));
        SocialPlugin *socioPlugin = static_cast<SocialPlugin *>(pluginInstance);
        return socioPlugin;
    }

    QStringList SocialDaemon::getPluginsList()
    {
        return PluginLoader::getInstance()->listPlugins(0);
    }

    QStringList SocialDaemon::supportedMethods(const QString &pluginName)
    {
        SocialPlugin *instance = getPluginInstance(pluginName);
        return instance->supportedMethods();//SocialPlugin.h must contain a method QStringList supportedMethods()
    }

    QVariantMap SocialDaemon::data(const QString &pluginName, const QString &methodName, QVariantMap args)
    {
        SocialPlugin *socioPlugin = getPluginInstance(pluginName);
        return socioPlugin->data(methodName, args);//SocialPlugin.h must contain a method QVariantMap data(const QString &methodName, QVariantMap args)
    }
}//namespace PlexyDesk

