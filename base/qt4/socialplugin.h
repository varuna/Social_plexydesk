
#ifndef PLEXY_SOCIAL_PLUGIN_H
#define PLEXY_SOCIAL_PLUGIN_H
#include <plexy.h>
#include <config.h>
#include <baseplugin.h>
#include <QMap>
#include <QObject>
#include <QStringList>
#include <QVariant>

namespace PlexyDesk
{
    class VISIBLE_SYM SocialPlugin : public BasePlugin
    {
        Q_OBJECT
    public :
        SocialPlugin(QObject  * object = 0): BasePlugin(object) {}
        virtual ~SocialPlugin() {}
    public slots:
      //  virtual void pushSocial(QVariant&) = 0;
        virtual QStringList supportedMethods()=0;
        virtual QVariantMap data(const QString &methodName, QVariantMap args)=0;
    };
}
#endif
