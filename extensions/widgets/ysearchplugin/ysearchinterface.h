#ifndef YSEARCHINTERFACE_H
#define YSEARCHINTERFACE_H

#include <QObject>
#include <QtPlugin>
#include <httpjobhandler.h>
class YSearchInterface
{

    public:
       virtual ~YSEARCHInterface(){}
       virtual QVariantMap searchResult()=0;
       virtual QVariantMap data(const QString &methodName, QVariantMap args)=0;
       virtual QStringList supportedMethods()=0;

};
//Q_DECLARE_INTERFACE(TwitterInterface, "org.socioplexy.Plugin.TwitterInterface/1.0");



#endif // YSEARCHINTERFACE_H
