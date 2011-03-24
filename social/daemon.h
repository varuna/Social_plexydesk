#include <QApplication>
#include <QDBusAbstractAdaptor>
#include <QObject>
#include <QDBusVariant>
#include <QList>
#include <QStringList>
#include <QVariantMap>
#include <socialplugin.h>
//<-----**********To be implemented*****************

namespace PlexyDesk
{
    class SocialDaemon : QDBusAbstractAdaptor
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", "local.socioplexy.Configuration")
        //Q_PROPERTY(QString display READ display)

    private:
        SocialPlugin* getPluginInstance(const QString &pluginName);
    public:
        SocialDaemon(QApplication *parent);
    public slots:
        QStringList getPluginsList();
        QStringList supportedMethods(const QString &pluginName);
        QVariantMap data(const QString &pluginName, const QString &methodName, QVariantMap args);


    };
} //namespace PlexyDesk
