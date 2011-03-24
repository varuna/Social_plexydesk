#include <QXmlQuery>
#include "ysearchplugin.h"
#include <QBuffer>
#include <QTest>
#include <QDebug>
#include <QtDBus/QtDBus>
#include <QDir>
#include <QXmlNamePool>

using namespace PlexyDesk;
struct YData
{
  QString title;
  QString summery;
  QString url;
  QString disp_url;
 };
Q_DECLARE_METATYPE(YData);
QDBusArgument &operator<<(QDBusArgument &argument, const YData &results)
{
        argument.beginStructure();
        argument << results.title <<results.summery<<results.url<<results.disp_url;
        argument.endStructure();
        return argument;
}
const QDBusArgument &operator>>(const QDBusArgument &argument, YData &results)
{
        argument.beginStructure();
        argument >> results.title >> results.summery >>results.url >>results.disp_url;
        argument.endStructure();
        return argument;
}
YSearchPlugin::YSearchPlugin(QObject * parent) : SocialPlugin(parent)
{   
     job = new HttpJobHandler(this);
     connect(job,SIGNAL(finished()),this,SLOT(listen()));
     qDBusRegisterMetaType<YData>();
     QUrl searchRes = QUrl("http://search.yahooapis.com/WebSearchService/V1/webSearch?appid=nm7Lg8zV34G0JE0aOt9f.pKi3KSrNqtow76AY7r5FQfloFkmcDle0dM40MdwAm5wqD7n&query=plexydesk&results=10");
     this->parseXmlData(searchRes);
}

void YSearchPlugin::listen()
{
    if (job->isError())
    {
        yError = job->errorName();
    }
    else
    {
        yahooXml = job->readData();
    }
}
void YSearchPlugin::getXml(const QUrl &url)
{
     qDebug()<<"YSearchPlugin::getXml() called";
     job->getFile(url);
     while(yahooXml.size() == 0){
         //job->getFile(url);
        QTest::qWait(1000);
      //  qDebug()<<"waiting";
    }
    //qDebug()<<yahooXml.isEmpty();
}

QVariantMap YSearchPlugin::parseXmlData(QUrl &url)
{
    qDebug()<<"im here";
    getXml(url);
    QBuffer buffer;
    buffer.setBuffer(&yahooXml);
    buffer.open(QIODevice::ReadOnly);
    QXmlQuery query;
    query.setFocus(&buffer);
    QStringList y_title,y_summery,y_url,y_dispUrl,tRR;
    query.setQuery("for $x in //*:ResultSet  return string($x /@totalResultsReturned)");
    query.evaluateTo(&tRR);

    query.setQuery("for $x in //*:ResultSet return string($x /Result/Title)");
    query.evaluateTo(&y_title);

    qDebug()<<y_title;
    //query.setQuery("for $x in //*:ResultSet  return string($x */Result/Summery)");
    //query.evaluateTo(&y_summery);
    //query.setQuery("for $x in /ResultSet/Result  return string($x /Url)");
    //query.evaluateTo(&y_url);
    //query.setQuery("for $x in /ResultSet/Result  return string($x /DisplayUrl)");
    //query.evaluateTo(&y_dispUrl);
    //qDebug()<<tRR<<"   --  "<<"----";
    //qDebug()<<y_title;
    /*QVariantMap yDt;
    for (int i =0;i<tRR.at(0).toInt();i++)
    {
        YData ysearch;
        ysearch.title = y_title.at(i);
        ysearch.summery = y_summery.at(i);
        ysearch.url = y_url.at(i);
        ysearch.disp_url = y_dispUrl.at(i);

        QVariant t;
        t.setValue(ysearch);
        yDt[QString::number(i)] = t;
    }
    */
    //qDebug() << yDt;
    //return yDt;
    //qDebug()<<tp;
    //qDebug()<<y_summery<<"   ----------came down";
    return QVariantMap();
}

QVariantMap  YSearchPlugin::searchResult(QVariantMap &args)
{
    QString query;
    if (args["query"].canConvert<QString>())
    {
        query  = args["query"].value<QString>();
    }
    else
    {
        query = "yahoo";
    }
    QUrl searchRes = QUrl("http://search.yahooapis.com/WebSearchService/V1/webSearch?appid=nm7Lg8zV34G0JE0aOt9f.pKi3KSrNqtow76AY7r5FQfloFkmcDle0dM40MdwAm5wqD7n&query=plexydesk&results=10");
    //QUrl searchRes = QUrl("http://api.guardianapis.com/content/search?q=world&api_key=gba692hup5jn84td66dpcpq5");
    return this->parseXmlData(searchRes);

}

QVariantMap YSearchPlugin::data(const QString &methodName, QVariantMap args)
{
    return this->searchResult(args);
}

QStringList YSearchPlugin::supportedMethods()
{
    QStringList t;
    t<<"searchResult";
    return t;
}
