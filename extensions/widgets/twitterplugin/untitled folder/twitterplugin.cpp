#include <QXmlQuery>
#include "twitterplugin.h"
#include <QBuffer>
#include <QTest>
#include <QDebug>
#include <QtDBus/QtDBus>
#include <QDir>

using namespace PlexyDesk;

const QUrl TwitterPlugin::PUBLIC_TIMELINE_URL("http://twitter.com/statuses/public_timeline.xml");
const QUrl TwitterPlugin::FRIENDS_TIMELINE_URL("http://twitter.com/statuses/friends_timeline.xml");
const QUrl TwitterPlugin::USER_TIMELINE_URL("http://twitter.com/statuses/user_timeline.xml");
const QUrl TwitterPlugin::USERNAME_MENTIONS_URL("http://twitter.com/statuses/mentions.xml");
const QUrl TwitterPlugin::SPECIFIC_STATUS_URL("http://twitter.com/statuses/show/id.xml");
const QUrl TwitterPlugin::STATUS_UPDATE_URL("http://twitter.com/statuses/update.xml");

struct TwitData
{
  QString status_id;
  QString screen_name;
  QString twit_text;
  QString avatar_path;
  //QByteArray img;
};
Q_DECLARE_METATYPE(TwitData);

QDBusArgument &operator<<(QDBusArgument &argument, const TwitData &status)
{
        argument.beginStructure();
        argument << status.status_id << status.screen_name << status.twit_text << status.avatar_path;// << status.img;
        argument.endStructure();
        return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, TwitData &status)
{
        argument.beginStructure();
        argument >> status.status_id >> status.screen_name >> status.twit_text >> status.avatar_path;// >> status.img;
        argument.endStructure();
        return argument;
}

TwitterPlugin::TwitterPlugin(QObject * parent) : SocialPlugin(parent)
{   
     job = new HttpJobHandler(this);
     connect(job,SIGNAL(finished()),this,SLOT(listen()));
     qDBusRegisterMetaType<TwitData>();

}



void TwitterPlugin::listen()
{
    if (job->isError())
    {
        twitterError = job->errorName();       
    }
    else
    {
        twitterXml = job->readData();
    }
}
void TwitterPlugin::getXml(const QUrl &url)
{
     job->getFile(url);
     while(twitterXml.size() == 0){
        QTest::qWait(1000);
    }
}

QVariantMap TwitterPlugin::parseXmlData(const QUrl &url)
{
    getXml(url);
    QBuffer buffer;
    buffer.setBuffer(&twitterXml);
    buffer.open(QIODevice::ReadOnly);

    QXmlQuery query;
    query.setFocus(&buffer);
    query.setQuery("for $x in /statuses/status return string($x/user/screen_name)");

    QStringList listScreenName, listStatus, avatarUrl, listStatusId;
    QString tweetOut;
    query.evaluateTo(&listScreenName);

    query.setQuery("for $x in /statuses/status return string($x/text)");
    query.evaluateTo(&listStatus);

    query.setQuery("for $x in /statuses/status return string($x/user/profile_image_url)");
    query.evaluateTo(&avatarUrl);

    query.setQuery("for $x in /statuses/status return string($x/id)");
    query.evaluateTo(&listStatusId);

    twitterXml.clear();

    QVariantMap result;

    for(int i=0; i<listStatusId.length(); i++){
        TwitData dat;
        dat.status_id = listStatusId.at(i);
        dat.twit_text = listStatus.at(i);
        dat.screen_name = listScreenName.at(i);
        //dat.avatar_path = getAvatar(avatarUrl.at(i));
        QVariant temp;
        temp.setValue(dat);
        result[listStatusId.at(i)] = temp;
    }
    qDebug() << result;
    return result;
}

QVariantMap TwitterPlugin::getPublicTimeline()
{
    return this->parseXmlData(PUBLIC_TIMELINE_URL);
}

QVariantMap TwitterPlugin::getFriendsTimeline(QVariantMap &args)
{
    QUrl temp=addUserCredentialsToUrl(FRIENDS_TIMELINE_URL, args) ;
    if(!temp.isEmpty())
        return this->parseXmlData(temp);
    return QVariantMap();
}
QVariantMap TwitterPlugin::getUserNameMentions(QVariantMap &args)
{
    QUrl temp=addUserCredentialsToUrl(USERNAME_MENTIONS_URL, args) ;
    if(!temp.isEmpty())
        return this->parseXmlData(temp);
    return QVariantMap();
}
QVariantMap TwitterPlugin::getUserTimeline(QVariantMap &args)
{
    QUrl temp=addUserCredentialsToUrl(USER_TIMELINE_URL, args) ;
    if(!temp.isEmpty())
        return this->parseXmlData(temp);
    return QVariantMap();
}
QString TwitterPlugin::getAvatar(const QUrl &url)
{
    twitterXml.clear();
    getXml(url);
    while(twitterXml.size() == 0){
        QTest::qWait(1000);
    }
    QImage img;
    img.loadFromData(twitterXml);
    QByteArray file;
    file.append(url.toString());
    QString fileName = file.toBase64();
    fileName = QDir::homePath()+"/.config/plexydesk/twitterplugin/"+fileName+".png";
    qDebug()<< "image file name: " << fileName;
    img.save(fileName);
    return fileName;
}

QUrl TwitterPlugin::addUserCredentialsToUrl(const QUrl &url, QVariantMap &args)
{

    if(((args["pwd"]).canConvert<QString>()) && ((args["uname"]).canConvert<QString>())){
        QUrl authorizedUrl(url);
        QString uname, pwd;
        uname = (args["uname"]).value<QString>();
        pwd = (args["pwd"]).value<QString>();
        authorizedUrl.setUserName(uname);
        authorizedUrl.setPassword(pwd);
        return authorizedUrl;
    }
    return QUrl();
}

void TwitterPlugin::updateStatus(QVariantMap &args)
{
    //this->inside_update_status = "I'm inside updateStatuse() method";
    QUrl tempUrl = addUserCredentialsToUrl(STATUS_UPDATE_URL, args);
    if (!tempUrl.isEmpty()){
        qDebug() << "TwitterPlugin::updateStatus()- url is not empty";
        if((args["status"]).canConvert<QString>()){
            qDebug() << "TwitterPlugin::updateStatus()- can convert uname & pwd";
            QString status = "status=";
            status += (args["status"]).value<QString>();

            QByteArray statusPost;
            statusPost.append(status);
            job->postFile(tempUrl, statusPost);

        }
    }
}

QVariantMap TwitterPlugin::data(const QString &methodName, QVariantMap args)
{
    QVariantMap sendData;
    if(methodName == "getPublicTimeline"){
        sendData = this->getPublicTimeline();
        //qDebug()<<"Getting public timeline";
        return sendData;
    }
    else if(methodName == "getFriendsTimeline"){
        sendData = this->getFriendsTimeline(args);
        return sendData;
    }
    else if(methodName == "updateStatus"){
        this->updateStatus(args);
        return sendData; //returns empty QVariantMap
    }
    else if (methodName == "getUserNameMentions"){
        sendData = this->getUserNameMentions(args);
        return sendData;
    }
    else if (methodName == "getUserTimeline"){
        sendData = this->getUserTimeline(args);
        return sendData;
    }
    else
        return sendData;
}

QStringList TwitterPlugin::supportedMethods()
{
    qDebug()<<"TwitterPlugin::supportedMethods() called";
    QStringList list;
    list <<"getPublicTimeline" << "getFriendsTimeline" << "updateStatus"<<"getUserTimeline"<<"getUserNameMentions";
    return list;
}
