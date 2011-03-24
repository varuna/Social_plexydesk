#include  "tweetsupplier.h"
#include <QString>
#include <QMap>
#include <QBitmap>
#include <QMatrix>
#include <QPaintEvent>
#include <QPixmap>
#include <QStringList>
#include <QDebug>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusInterface>
#include <QDBusVariant>
#include <QDBusArgument>
#include <QtDBus>
#include <QDBusConnectionInterface>
#include <QDBusAbstractInterface>
#include <QDBusReply>
TweetSupplier::TweetSupplier(){}
TweetSupplier::~TweetSupplier(){}
QVariantMap TweetSupplier::getPublications(QString pluginMethod)
{

    //connecting the QDBus with the widget
        QDBusMessage busMsg = QDBusMessage::createMethodCall("org.plexydesk.social", "/services","local.socioplexy.Configuration","getPluginsList");

        //creating the QDBus connection with the sessionBus.
        QDBusConnection bus = QDBusConnection::connectToBus(QDBusConnection::SessionBus, "socioplexy");

        /*Bus connection Checking..
        if(bus.isConnected())
                qDebug() << "QDBusConnection is successfull";
        else
            qDebug()<<"QDBusConnection is unsuccessfull";*/

        //listening for the reply from the QDBus
        QDBusPendingReply<QStringList> msgRep = bus.asyncCall(busMsg,10000);
        msgRep.waitForFinished();
        //if reply is valid only proceed the QDBus connection method calling
        if (msgRep.isValid())
        {
                QStringList plugins = msgRep.value();
                if (!plugins.isEmpty())
                {
                    int i=0,j;
                    //checking for the twitterplugin in the plugins list loaded.
                    for (j=0;j<plugins.count();j++)
                    {
                        if (plugins.value(j)=="weatherplugin"){
                            i = 1;  //Flag for plugin found
                            break;
                        }
                    }

                    //if plugin twitterplugin is found then request data from the plugin
                    if (i == 1)
                    {
                        QString pluginName = plugins.value(j);
                        QDBusMessage pluginBusMsg;
                        pluginBusMsg = QDBusMessage::createMethodCall("org.plexydesk.social", "/services","local.socioplexy.Configuration","supportedMethods");
                        QList<QVariant> argsSuppMethods;
                        argsSuppMethods << QVariant(pluginName);
                        pluginBusMsg.setArguments(argsSuppMethods);
                        qDebug()<<pluginBusMsg.arguments();
                        QDBusPendingReply<QStringList> supportMsgRep = bus.asyncCall(pluginBusMsg,1000);
                        supportMsgRep.waitForFinished();

                        if (supportMsgRep.isValid())
                        {
                            QStringList supportedMethodsList = supportMsgRep.value();
                            if(supportedMethodsList.contains(pluginMethod))
                            {
                                QDBusMessage getDataMsg;
                                QVariantMap args;
                                QList<QVariant> argsData;
                                getDataMsg = QDBusMessage::createMethodCall("org.plexydesk.social", "/services", "local.socioplexy.Configuration", "data");
                                argsData << QVariant("weatherplugin") << QVariant(pluginMethod) << QVariant(args);
                                getDataMsg.setArguments(argsData);
                                QDBusPendingReply<QVariantMap> recvData = bus.asyncCall(getDataMsg,10000);
                                recvData.waitForFinished();
                                if(recvData.isValid())
                                    return recvData.value();
                                else
                                {
                                    QVariantMap dummy;
                                    dummy["Network Error"] = "Data not recieved";
                                    return dummy;
                                }
                            }
                            else
                                qDebug() << "supportedMethods doesn't contain getPublicTimeline";
                        }
                        else
                        {
                            qDebug()<<"Error reply "<<supportMsgRep.error();
                        }
                    }
                    else
                    {
                        qDebug()<<"No matching plugin loaded...";
                    }
                }
                else
                {
                    qDebug()<<"No plugin name was retireved.";
                }
        }
        else
        {
                qDebug()<<"error reply"<<msgRep.error();
        }
}
bool TweetSupplier::getAuthentication(QString username, QString password)
{
    return true;
}