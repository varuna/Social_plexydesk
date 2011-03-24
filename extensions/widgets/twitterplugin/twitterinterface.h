#ifndef TWITTERINTERFACE_H
#define TWITTERINTERFACE_H

#include <QObject>
#include <QtPlugin>
#include <httpjobhandler.h>
class TwitterInterface
{

    public:
       virtual ~TwitterInterface(){}
       virtual QVariantMap getPublicTimeline()=0;
       virtual QVariantMap data(const QString &methodName, QVariantMap args)=0;
       virtual QStringList supportedMethods()=0;

};
//Q_DECLARE_INTERFACE(TwitterInterface, "org.socioplexy.Plugin.TwitterInterface/1.0");



#endif // TWITTERINTERFACE_H
