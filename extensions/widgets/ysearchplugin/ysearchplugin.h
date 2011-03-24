#ifndef YSEARCHPLUGIN_H
#define YSEARCHPLUGIN_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <httpjobhandler.h>
#include <baseplugin.h>
#include <abstractplugininterface.h>
#include <socialplugin.h>

class YSearchPlugin : public PlexyDesk::SocialPlugin
{

    Q_OBJECT;
 private:
    PlexyDesk::HttpJobHandler *job;
    QByteArray yahooXml;
    QString yError;
    static const QUrl SEARCH_URL;

    //test suite
    QString inside_update_status;

private:
    void getXml(const QUrl &url);
    QVariantMap parseXmlData(QUrl &url);
    QVariantMap getPublicTimeline();
    QVariantMap searchResult(QVariantMap &args);
   
   

 public:
    YSearchPlugin(QObject *parent=0);
    QStringList supportedMethods();
    QVariantMap data(const QString &methodName, QVariantMap args);

private slots:
    void listen();

};

#endif // YSEARCHPLUGIN_H
