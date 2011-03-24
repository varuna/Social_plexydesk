#ifndef TWITTERPLUGIN_H
#define TWITTERPLUGIN_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <httpjobhandler.h>
#include <baseplugin.h>
#include <abstractplugininterface.h>
#include <socialplugin.h>

class TwitterPlugin : public PlexyDesk::SocialPlugin
{

    Q_OBJECT;
 private:
    QString twitterData;
    PlexyDesk::HttpJobHandler *job;
    QByteArray twitterXml;
    QString twitterError;
    QString twitterErrorMsg;
    static const QString cachePath;
    static const QUrl PUBLIC_TIMELINE_URL;
    static const QUrl FRIENDS_TIMELINE_URL;
    static const QUrl USER_TIMELINE_URL;
    static const QUrl USERNAME_MENTIONS_URL;
    static const QUrl SPECIFIC_STATUS_URL;
    static const QUrl STATUS_UPDATE_URL;
    static const QUrl END_SESSION_URL;

    //test suite
    QString inside_update_status;

private:
    void getXml(const QUrl &url);
    void postXml(const QUrl &url);
    QVariantMap parseXmlData(const QUrl &url);
    QVariantMap getPublicTimeline();
    QVariantMap getFriendsTimeline(QVariantMap &args);
    QVariantMap getUserTimeline(QVariantMap &args);
    QVariantMap getUserNameMentions(QVariantMap &args);
    QString getAvatar(const QString &url);
    QVariantMap updateStatus(QVariantMap &args);
    QVariantMap endSession(QVariantMap &args);
    QUrl addUserCredentialsToUrl(const QUrl &url, QVariantMap &args);
    QString checkAvatarAvailability(const QString &url);

 public:
    TwitterPlugin(QObject *parent=0);
    QStringList supportedMethods();
    QVariantMap data(const QString &methodName, QVariantMap args);

private slots:
    void listen();

};

#endif // TWITTERPLUGIN_H
