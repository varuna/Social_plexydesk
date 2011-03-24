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
const QUrl TwitterPlugin::END_SESSION_URL("http://twitter.com/account/end_session.xml");

const QString TwitterPlugin::cachePath(QDir::homePath() + "/.config/plexydesk/social/daemon/image/cache/");

struct TwitData
{
  QString status_id;
  QString screen_name;
  QString twit_text;
  QString avatar_path;
  QString twit_error_msg;
  bool twit_is_error;
  //QByteArray img;
};
Q_DECLARE_METATYPE(TwitData);

QDBusArgument &operator<<(QDBusArgument &argument, const TwitData &status)
{
        argument.beginStructure();
        argument << status.status_id << status.screen_name << status.twit_text << status.avatar_path << status.twit_error_msg << status.twit_is_error;// << status.img;
        argument.endStructure();
        return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, TwitData &status)
{
        argument.beginStructure();
        argument >> status.status_id >> status.screen_name >> status.twit_text >> status.avatar_path >> status.twit_error_msg >> status.twit_is_error ;// >> status.img;
        argument.endStructure();
        return argument;
}

TwitterPlugin::TwitterPlugin(QObject * parent) : SocialPlugin(parent)
{   
     job = new HttpJobHandler(this);
     connect(job,SIGNAL(finished()),this,SLOT(listen()));
     qDBusRegisterMetaType<TwitData>();
     QDir cacheDir;
      if (!QDir(cachePath).exists())
        cacheDir.mkpath(this->cachePath);

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
    //reset variables
    this->twitterXml.clear();
    this->twitterError.clear();
    this->twitterErrorMsg.clear();

     job->getFile(url);
     while(twitterXml.size() == 0){
        QTest::qWait(1000);
    }
}

void TwitterPlugin::postXml(const QUrl &url)
{
    //reset variables
    this->twitterXml.clear();
    this->twitterError.clear();
    this->twitterErrorMsg.clear();

    job->postFile(url, QByteArray());
    while(twitterXml.size() == 0)
            QTest::qWait(1000);
}

QVariantMap TwitterPlugin::parseXmlData(const QUrl &url)
{
    getXml(url);
    QBuffer buffer;
    buffer.setBuffer(&twitterXml);
    buffer.open(QIODevice::ReadOnly);

    QXmlQuery query;
    query.setFocus(&buffer);

    qDebug()<<"for $x in /statuses/status return string($x/user/screen_name)";
    query.setQuery("for $x in /statuses/status return string($x/user/screen_name)");

    QStringList listScreenName, listStatus, avatarUrl, listStatusId;
    QString tweetOut;
    query.evaluateTo(&listScreenName);
    qDebug()<<endl<<"screenNames output :"<<endl<<listScreenName;

    query.setQuery("for $x in /statuses/status return string($x/text)");
    query.evaluateTo(&listStatus);

    query.setQuery("for $x in /statuses/status return string($x/user/profile_image_url)");
    query.evaluateTo(&avatarUrl);

    query.setQuery("for $x in /statuses/status return string($x/id)");
    query.evaluateTo(&listStatusId);
    QVariantMap result;
    qDebug()<<endl<<"Picture settings :"<<endl;
    for(int i=0; i<listStatusId.length(); i++){
        TwitData dat;
        dat.twit_is_error = false;
        dat.status_id = listStatusId.at(i);
        dat.twit_text = listStatus.at(i);
        dat.screen_name = listScreenName.at(i);
        QString fileExists = checkAvatarAvailability(avatarUrl.at(i));
        qDebug()<<endl<<avatarUrl.at(i);
        if(fileExists == "File Not Found")
            dat.avatar_path = getAvatar(avatarUrl.at(i));
        else
            dat.avatar_path = fileExists;
        QVariant temp;
        temp.setValue(dat);
        result[listStatusId.at(i)] = temp;
    }
    if(listStatusId.length() == 0){
        TwitData emptyDat;
        QVariant temp;
        temp.setValue(emptyDat);
        result["first"] = temp;
    }
    /*
    ///-----------------
    qDebug()<<"ok here";
    QUrl test = QUrl("http://search.yahooapis.com/WebSearchService/V1/webSearch?appid=nm7Lg8zV34G0JE0aOt9f.pKi3KSrNqtow76AY7r5FQfloFkmcDle0dM40MdwAm5wqD7n&query=plexydesk&results=10");
    getXml(url);
    QBuffer buffer1;
    buffer1.setBuffer(&twitterXml);
    buffer1.open(QIODevice::ReadOnly);
    //query;
    query.setFocus(&buffer1);
    QStringList tp;
    query.setQuery("for $x in /resultset/result  return string($x /title)");
    qDebug()<<query.isValid();
    query.evaluateTo(&listScreenName);
    ///------------------------------
    */
    return result;
}

QVariantMap TwitterPlugin::getPublicTimeline()
{
    return this->parseXmlData(PUBLIC_TIMELINE_URL);
}

QVariantMap TwitterPlugin::getFriendsTimeline(QVariantMap &args)
{
    QUrl temp=addUserCredentialsToUrl(FRIENDS_TIMELINE_URL, args);
    if(!temp.isEmpty())
        return this->parseXmlData(temp);
    return QVariantMap();
}
QVariantMap TwitterPlugin::getUserNameMentions(QVariantMap &args)
{
    TwitData dat;
    QUrl temp=addUserCredentialsToUrl(USERNAME_MENTIONS_URL, args) ;
    if(!temp.isEmpty()){
        return this->parseXmlData(temp);
    }
    else{ // if QUrl is returned empty

    }
    return QVariantMap();
}
QVariantMap TwitterPlugin::getUserTimeline(QVariantMap &args)
{
    QUrl temp=addUserCredentialsToUrl(USER_TIMELINE_URL, args) ;
    if(!temp.isEmpty())
        return this->parseXmlData(temp);
    return QVariantMap();
}
QString TwitterPlugin::getAvatar(const QString &url)
{
    QUrl tempUrl(url);
    twitterXml.clear();
    getXml(tempUrl);
    while(twitterXml.size() == 0){
        QTest::qWait(1000);
    }
    QImage img;
    img.loadFromData(twitterXml);
    QByteArray file;
    file.append(url);
    QString fileName = file.toBase64();
    fileName = this->cachePath + fileName + ".png";
    qDebug()<< "image file name: " << fileName;
    img.save(fileName);
    return fileName;
}

QString TwitterPlugin::checkAvatarAvailability(const QString &url)
{
     QByteArray byteArrayFile;
     byteArrayFile.append(url);
     QString fileNameString = byteArrayFile.toBase64() + ".png";
     QDir cacheDir(this->cachePath);
     if(cacheDir.exists(fileNameString))
         return (this->cachePath + fileNameString);
     else
         return "File Not Found";
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

QVariantMap TwitterPlugin::updateStatus(QVariantMap &args)
{
    TwitData dat;
    qDebug()<<"I'm inside updateStatuse() method in TwiiterPlugin";
    QUrl tempUrl = addUserCredentialsToUrl(STATUS_UPDATE_URL, args);
    if (!tempUrl.isEmpty()){
        if((args["status"]).canConvert<QString>()){
            QString status = (args["status"]).value<QString>();
            tempUrl.addQueryItem("status", status);
            //QByteArray statusPost;
            //job->postFile(tempUrl, statusPost); //send an empty QByteArray;
            postXml(tempUrl);
            if(this->twitterError == "200"){
                dat.twit_is_error = false;
            }
            else{
                    dat.twit_is_error = true;
                    dat.twit_error_msg = this->twitterError;
                }
        }
    }
    else{ //If the Url is returned empty
        dat.twit_is_error = true;
        dat.twit_error_msg = "Unable to authenticate user!";
    }
    QVariantMap send;
    QVariant returnData;
    returnData.setValue(dat);
    send["feedBack"] = returnData;
    return send;

}

QVariantMap TwitterPlugin::endSession(QVariantMap &args)
{
    TwitData dat;
    QUrl tempUrl = addUserCredentialsToUrl(END_SESSION_URL, args);
    if(!tempUrl.isEmpty()){
        postXml(tempUrl);
        if(this->twitterError == "200")
            dat.twit_is_error = false;
        else{
            dat.twit_is_error = true;
            dat.twit_error_msg = this->twitterErrorMsg;
        }
    }
    else{
        dat.twit_is_error = true;
        dat.twit_error_msg = "unable to authenticate user!";
    }
    QVariantMap send;
    QVariant returnData;
    returnData.setValue(dat);

}

QVariantMap TwitterPlugin::data(const QString &methodName, QVariantMap args)
{
    //QVariantMap sendData;
    if(methodName == "getPublicTimeline")
        return this->getPublicTimeline();
    else if(methodName == "getFriendsTimeline")
        return this->getFriendsTimeline(args);
    else if(methodName == "updateStatus")
        return this->updateStatus(args);
    else if (methodName == "getUserNameMentions")
        return this->getUserNameMentions(args);
    else if (methodName == "getUserTimeline")
        return this->getUserTimeline(args);
    else if(methodName == "endSession"){
        this->endSession(args);
        return QVariantMap();
    }
    else
        return QVariantMap();
}

QStringList TwitterPlugin::supportedMethods()
{
    qDebug()<<"TwitterPlugin::supportedMethods() called";
    QStringList list;
    list <<"getPublicTimeline" << "getFriendsTimeline" << "updateStatus"<<"getUserTimeline"<<"getUserNameMentions"<<"endSession";
    return list;
}
