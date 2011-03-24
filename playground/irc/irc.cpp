#include "irc.h"
#include <QApplication>
#include <QNetworkProxy>
#include <iostream>

IrcData::IrcData(QObject *p) : QObject(p)
{
    this->Connected = 0;
}

IrcData::IrcData(QString server_arg, qint16 port_arg)
{
    Connected = 0;
    server = server_arg;
    port = port_arg;
}

bool connectCalled=0;
bool partCalled=0;
bool nickCalled =0;
bool userCalled =0;
bool joinCalled =0;
bool privmsgCalled =0;
bool passCalled =0;
bool inviteCalled =0;
bool quitCalled =0;
bool kickCalled =0;
bool whoisCalled =0;
bool awayCalled = 0;
bool operCalled =0;
bool modeCalled =0;
bool listCalled =0;
bool topicCalled =0;
bool noticeCalled =0;
bool namesCalled = 0;

void IrcData::connectToServer()
{
    service = new QTcpSocket();

    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::HttpProxy);
    proxy.setHostName("vsnlproxy.iitk.ac.in");
    proxy.setPort(3128);
    proxy.setUser("shankar");
    proxy.setPassword("LINKINpark");

    service->setProxy(proxy);
    service->connectToHost(server,port);

    connect(service,SIGNAL(error(QAbstractSocket::SocketError)),SLOT(errorHandler(QAbstractSocket::SocketError)));
    connect(service,SIGNAL(connected()),SLOT(setConnected()));

    connect(service,SIGNAL(readyRead()),SLOT(parse()));
}

void IrcData::setNick(QString nick)
{
    service->write(QString("NICK %1\r\n").arg(nick).toAscii());
    nickCalled = 1;
}

void IrcData::setUser(QString user,qint16 mode,QString unused,QString realName)
{
    service->write(QString("USER %1 %2 %3 :%4\r\n").arg(user).arg(mode).arg(unused).arg(realName).toAscii());
    userCalled = 1;
}

void IrcData::joinChannel(QString channel)
{
    service->write(QString("JOIN %1\r\n").arg(channel).toAscii());
    joinCalled = 1;
}

void IrcData::writeMessage(QString channel,QString message)
{
    service->write(QString("PRIVMSG %1 :%2\r\n").arg(channel).arg(message).toAscii());
    privmsgCalled = 1;
}

void IrcData::partChannel(QString channel,QString message)
{
    service->write(QString("PART %1 :%2\r\n").arg(channel).arg(message).toAscii());
    partCalled = 1;
}

void IrcData::setPassword(QString pass)
{
    service->write(QString("PASS %1\r\n").arg(pass).toAscii());
    passCalled = 1;
}

void IrcData::invite(QString nick,QString channel)
{
    service->write(QString("INVITE %1 %2\r\n").arg(nick).arg(channel).toAscii());
    inviteCalled = 1;
}

void IrcData::quit(QString message)
{
    service->write("QUIT :Yay sira!\r\n");
//     service->write(QString("QUIT :Yay , sharp's QUIT works!\r\n")/*.arg(message)*/.toAscii());
    quitCalled = 1;
}

void IrcData::kick(QString channel,QString nick,QString message)
{
    service->write(QString("KICK %1 %2 :%3\r\n").arg(channel).arg(nick).arg(message).toAscii());
    kickCalled = 1;
}

void IrcData::whois(QString nick)
{
    service->write(QString("WHOIS %1\r\n").arg(nick).toAscii());
    whoisCalled = 1;
}

void IrcData::away(QString message)
{
    service->write(QString("AWAY %1\r\n").arg(message).toAscii());
    awayCalled = 1;
}

void IrcData::oper(QString nick,QString password)
{
    service->write(QString("OPER %1 %2\r\n").arg(nick).arg(password).toAscii());
    operCalled = 1;
}

void IrcData::mode(QString nick,QString mode)
{
    service->write(QString("MODE  %1 %2\r\n").arg(nick).arg(mode).toAscii());
    modeCalled = 1;
}

void IrcData::topic(QString channel,QString topic)
{
    service->write(QString("TOPIC %1 :%2\r\n").arg(channel).arg(topic).toAscii());
    topicCalled = 1;
}

void IrcData::list()
{
    service->write(QString("LIST\r\n").toAscii());
    listCalled = 1;
}

void IrcData::notice(QString target,QString text)
{
    service->write(QString("NOTICE %1 :%2\r\n").arg(target).arg(text).toAscii());
    noticeCalled = 1;
}

void IrcData::names(QString channel)
{
    service->write(QString("NAMES %1\r\n").arg(channel).toAscii());
    namesCalled = 1;
}

// void IrcData::init()
// {
//     service->write("NICK sharpBot\r\n");
//     service->write("USER sharpBot 7 * :Mani Shankar's BOT\r\n");
//     service->write("JOIN #plexydesk\r\n");
// }

QString getArg1(QString* currentLine){
    QString *arg1;
    QString *restLine;
    QRegExp argRegExp("([^\\s]*)[\\s].*");
    int pos = argRegExp.indexIn(*currentLine);
    if(pos>-1){
        arg1 = new QString(argRegExp.cap(1));
    }
}

QString getRestLine1(QString* currentLine){
    QString *arg1;
    QString *restLine;
    QRegExp argRegExp("([^\\s]*)[\\s].*");
    int pos = argRegExp.indexIn(*currentLine);
    if(pos>-1){
        arg1 = new QString(argRegExp.cap(1));
    }
    QRegExp restRegExp("[^\\s]*[\\s](.*)");
    pos = restRegExp.indexIn(*currentLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    restLine->remove(0,1);
    restLine->remove("\r\n");
    return *restLine;
}

QString getArg2(QString* currentLine){
    QString *arg1;
    QString *arg2;
    QString *restLine;
    QRegExp argRegExp("([^\\s]*)[\\s].*");
    int pos = argRegExp.indexIn(*currentLine);
    if(pos>-1){
        arg1 = new QString(argRegExp.cap(1));
    }
    QRegExp restRegExp("[^\\s]*[\\s](.*)");
    pos = restRegExp.indexIn(*currentLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg2 = new QString(argRegExp.cap(1));
    }
    return *arg2;
}

QString getRestLine2(QString* currentLine){
    QString *arg1;
    QString *arg2;
    QString *restLine;
    QRegExp argRegExp("([^\\s]*)[\\s].*");
    int pos = argRegExp.indexIn(*currentLine);
    if(pos>-1){
        arg1 = new QString(argRegExp.cap(1));
    }
    QRegExp restRegExp("[^\\s]*[\\s](.*)");
    pos = restRegExp.indexIn(*currentLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg2 = new QString(argRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    restLine->remove(0,1);
    restLine->remove("\r\n");
    return *restLine;
}

QString getArg3(QString* currentLine){
    QString *arg1;
    QString *arg2;
    QString *arg3;
    QString *restLine;
    QRegExp argRegExp("([^\\s]*)[\\s].*");
    int pos = argRegExp.indexIn(*currentLine);
    if(pos>-1){
        arg1 = new QString(argRegExp.cap(1));
    }
    QRegExp restRegExp("[^\\s]*[\\s](.*)");
    pos = restRegExp.indexIn(*currentLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg2 = new QString(argRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg3 = new QString(argRegExp.cap(1));
    }
    return *arg3;
}

QString getRestLine3(QString* currentLine){
    QString *arg1;
    QString *arg2;
    QString *arg3;
    QString *restLine;
    QRegExp argRegExp("([^\\s]*)[\\s].*");
    int pos = argRegExp.indexIn(*currentLine);
    if(pos>-1){
        arg1 = new QString(argRegExp.cap(1));
    }
    QRegExp restRegExp("[^\\s]*[\\s](.*)");
    pos = restRegExp.indexIn(*currentLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg2 = new QString(argRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg3 = new QString(argRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    restLine->remove(0,1);
    restLine->remove("\r\n");
    return *restLine;
}

QString getArg4(QString* currentLine){
    QString *arg1;
    QString *arg2;
    QString *arg3;
    QString *arg4;
    QString *restLine;
    QRegExp argRegExp("([^\\s]*)[\\s].*");
    int pos = argRegExp.indexIn(*currentLine);
    if(pos>-1){
        arg1 = new QString(argRegExp.cap(1));
    }
    QRegExp restRegExp("[^\\s]*[\\s](.*)");
    pos = restRegExp.indexIn(*currentLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg2 = new QString(argRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg3 = new QString(argRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg4 = new QString(argRegExp.cap(1));
    }
    return *arg4;
}

QString getRestLine4(QString* currentLine){
    QString *arg1;
    QString *arg2;
    QString *arg3;
    QString *arg4;
    QString *restLine;
    QRegExp argRegExp("([^\\s]*)[\\s].*");
    int pos = argRegExp.indexIn(*currentLine);
    if(pos>-1){
        arg1 = new QString(argRegExp.cap(1));
    }
    QRegExp restRegExp("[^\\s]*[\\s](.*)");
    pos = restRegExp.indexIn(*currentLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg2 = new QString(argRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg3 = new QString(argRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg4 = new QString(argRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    restLine->remove(0,1);
    restLine->remove("\r\n");
    return *restLine;
}

QString getArg5(QString* currentLine){
    QString *arg1;
    QString *arg2;
    QString *arg3;
    QString *arg4;
    QString *arg5;
    QString *restLine;
    QRegExp argRegExp("([^\\s]*)[\\s].*");
    int pos = argRegExp.indexIn(*currentLine);
    if(pos>-1){
        arg1 = new QString(argRegExp.cap(1));
    }
    QRegExp restRegExp("[^\\s]*[\\s](.*)");
    pos = restRegExp.indexIn(*currentLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg2 = new QString(argRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg3 = new QString(argRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg4 = new QString(argRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg5 = new QString(argRegExp.cap(1));
    }
    return *arg5;
}

QString getRestLine5(QString* currentLine){
    QString *arg1;
    QString *arg2;
    QString *arg3;
    QString *arg4;
    QString *arg5;
    QString *restLine;
    QRegExp argRegExp("([^\\s]*)[\\s].*");
    int pos = argRegExp.indexIn(*currentLine);
    if(pos>-1){
        arg1 = new QString(argRegExp.cap(1));
    }
    QRegExp restRegExp("[^\\s]*[\\s](.*)");
    pos = restRegExp.indexIn(*currentLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg2 = new QString(argRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg3 = new QString(argRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg4 = new QString(argRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg5 = new QString(argRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    restLine->remove(0,1);
    restLine->remove("\r\n");
    return *restLine;
}

QString getArg6(QString *currentLine){
    QString *arg1;
    QString *arg2;
    QString *arg3;
    QString *arg4;
    QString *arg5;
    QString *arg6;
    QString *restLine;
    QRegExp argRegExp("([^\\s]*)[\\s].*");
    int pos = argRegExp.indexIn(*currentLine);
    if(pos>-1){
        arg1 = new QString(argRegExp.cap(1));
    }
    QRegExp restRegExp("[^\\s]*[\\s](.*)");
    pos = restRegExp.indexIn(*currentLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg2 = new QString(argRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg3 = new QString(argRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg4 = new QString(argRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg5 = new QString(argRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg6 = new QString(argRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    return *arg6;
}

QString getRestLine6(QString *currentLine){
    QString *arg1;
    QString *arg2;
    QString *arg3;
    QString *arg4;
    QString *arg5;
    QString *arg6;
    QString *restLine;
    QRegExp argRegExp("([^\\s]*)[\\s].*");
    int pos = argRegExp.indexIn(*currentLine);
    if(pos>-1){
        arg1 = new QString(argRegExp.cap(1));
    }
    QRegExp restRegExp("[^\\s]*[\\s](.*)");
    pos = restRegExp.indexIn(*currentLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg2 = new QString(argRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg3 = new QString(argRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg4 = new QString(argRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg5 = new QString(argRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = argRegExp.indexIn(*restLine);
    if(pos>-1){
        arg6 = new QString(argRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    pos = restRegExp.indexIn(*restLine);
    if(pos>-1){
        restLine = new QString(restRegExp.cap(1));
    }
    restLine->remove(0,1);
    restLine->remove("\r\n");
    return *restLine;
}


int i=1;
int j =0;
int listFinish = 1;
QStringList empty; //UserList
User who; //Whois List
bool append = 0;
char buffer[1024];
char tempBuffer[1024];
QString *host;
void IrcData::parse()
{
    QString *currentLine;
//     qDebug() << service->readAll();
    while(service->readLine(buffer,sizeof(buffer)) > 0){
        if(QString(buffer).endsWith("\r\n")){
            if(append){
                currentLine = new QString(tempBuffer);
                currentLine->append(QString(buffer));
                append = 0;
            }
            else {
                currentLine = new QString(buffer);
            }
        }
        else{
            for(int i=0; i<1024;i++)
                tempBuffer[i] = buffer[i];
            append = 1;
            continue;
        }
        QString *arg1;
        QString *arg2;
        QString *arg3;
        QString *arg4;
        QString *arg5;
        QString *arg6;
        qDebug() << *currentLine;
        if((currentLine->left(1)).compare(":")==0){
                currentLine->remove(0,1);   // remove the prefix :
                QRegExp argRegExp("([^\\s]*)[\\s].*");
                int pos = argRegExp.indexIn(*currentLine);
                if(pos>-1){
                    arg1 = new QString(argRegExp.cap(1));
                }
                QString *restLine;
                QRegExp restRegExp("[^\\s]*[\\s](.*)");
                pos = restRegExp.indexIn(*currentLine);
                if(pos>-1){
                    restLine = new QString(restRegExp.cap(1));
                }
                pos = argRegExp.indexIn(*restLine);
                if(pos>-1){
                    arg2 = new QString(argRegExp.cap(1));
                }
                if(i==4 || i==5){
                    host = new QString(*arg1);
                }
                if(  arg1->compare(*host) == 0)
                {
//                     qDebug() << *arg2;
                    switch(arg2->toInt()){
                        case 1: 
                            if(userCalled){
                                emit userResponse(UserOK, "User OK");
                                userCalled = 0;
                            }
                            if(nickCalled){
                                emit nickResponse(NickOK, "Nick OK");
                                nickCalled = 0;
                            }
                            break;
                        case 301:
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg3 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(privmsgCalled){
                                emit messageResponse(AwayMessage,*restLine);
                                privmsgCalled = 0;
                            }
                            if(inviteCalled){
                                emit inviteResponse(InviteAwayMessage,*restLine);
                                inviteCalled = 0;
                            }
                            if(whoisCalled){
                                who.isAway =1;
                                who.awayMessage = *restLine;
//                                 whoisCalled = 0;
                            }
//                             if(awayCalled){
//                                 emit awayResponse(Away,"Away OK");
//                                 awayCalled = 0;
//                             }
                            break;
                        case 305:
                            if(awayCalled){
                                emit awayResponse(UnAway,"UnAway OK");
                                awayCalled = 0;
                            }
                            break;
                        case 306:
                            if(awayCalled){
                                emit awayResponse(Away,"Away OK");
                                awayCalled = 0;
                            }
                            break;
                        case 311:
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg3 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg4 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg5 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg6 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            restLine->remove(0,1);
                            restLine->remove("\r\n");
                            if(whoisCalled){
                                who.nick =*arg4;
                                who.user = *arg5;
                                who.host = *arg6;
                                who.realName = *restLine;
//                                 qDebi
//                                 whoisCalled = 0;
                            }
                            break;
                        case 312:
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg3 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg4 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg5 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            restLine->remove(0,1);
                            restLine->remove("\r\n");
                            if(whoisCalled){
                                who.server =*arg5;
                                who.serverInfo = *restLine;
//                                 whoisCalled = 0;
                            }
                            break;
                        case 313:
                            if(whoisCalled){
                                who.isOP = 1;
//                                 whoisCalled = 0;
                            }
                            break;
                        case 317:
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg3 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg4 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg5 = new QString(argRegExp.cap(1));
                            }
                            if(whoisCalled){
                                who.isIdle = 1;
                                who.idleTime = *arg5;
//                                 whoisCalled = 0;
                            }
                            break;
                        case 318:
                            if(whoisCalled){
                                emit whoisResponse(WhoisOK,"Whois OK",who);
                                who = User();
                                whoisCalled = 0;
                            }
                            break;
                        case 319:
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg3 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg4 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
//                             qDebug() << *currentLine;
                            restLine->remove(0,1);
                            restLine->remove("\r\n");
//                             qDebug() << *restLine;
                            if(whoisCalled){
                                while(restLine->compare("")!=0 || restLine->compare(" ")!=0){
                                    pos = argRegExp.indexIn(*restLine);
                                    if(pos>-1){
                                        who.channels << QString(argRegExp.cap(1));
//                                         qDebug() << QString(argRegExp.cap(1));
                                    }
                                    pos = restRegExp.indexIn(*restLine);
                                    if(pos>-1){
                                        restLine = new QString(restRegExp.cap(1));
//                                         qDebug() << *restLine;
                                    }
                                    else {break;}
                                    j++;
                                }
//                                 whoisCalled = 0;
                            }
                            break;
                        case 322:
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg3 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            restLine->remove("\r\n");
                            if(listCalled){
                                empty << *restLine;
                            }
                            break;
                        case 323:
                            if(listCalled){
                                emit listResponse(ListOK,"List OK",empty);
                                empty = QStringList();
                                listCalled = 0;
                            }
                            break;
                        case 331:
                            if(topicCalled){
                                emit topicResponse(TopicNotSet,"Topic No Topic Set");
                                topicCalled = 0;
                            }
                            break;
                        case 332:
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg3 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg4 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            restLine->remove(0,1);
                            restLine->remove("\r\n");
                            if(joinCalled){
                                emit channelResponse(Topic,*restLine,empty << *arg4);
                                joinCalled = 0;
                            }
                            if(topicCalled){
                                emit topicResponse(TopicSet,*restLine);
                                topicCalled = 0;
                            }
                            break;
                        case 341:
                            if(inviteCalled){
                                emit inviteResponse(InviteOK,"Invite OK");
                                inviteCalled = 0;
                            }
                            break;
                        case 353:
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg3 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg4 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg5 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            restLine->remove(0,1);
                            restLine->remove("\r\n");
//                             qDebug() << QString("%1 %2 %3 %4 %5 :").arg(*arg1).arg(*arg2).arg(*arg3).arg(*arg4).arg(*arg5);
//                             while(j<10){
                            while(restLine->compare("")!=0 || restLine->compare(" ")!=0){
                                pos = argRegExp.indexIn(*restLine);
                                if(pos>-1){
                                    empty << QString(argRegExp.cap(1));
//                                     qDebug() << QString(argRegExp.cap(1));
                                }
                                pos = restRegExp.indexIn(*restLine);
                                if(pos>-1){
                                    restLine = new QString(restRegExp.cap(1));
//                                     qDebug() << *restLine;
                                }
                                else {break;}
                                j++;
                            }
//                             qDebug() << empty << "HHHH******";
                            break;
                        case 366:
//                             listFinish = 1;
                            if(joinCalled){
                                emit channelResponse(UserList,"user List",empty);
                                empty = QStringList();
                                joinCalled = 0;
                                qDebug("JOIN Called");
                            }
                            if(namesCalled){
                                emit namesResponse(NamesUserList,"Names user List",empty);
                                empty = QStringList();
                                namesCalled = 0;
                                qDebug("NAMES called");
                            }
//                             qDebug() << empty;
                            break;
                        case 381:
                            if(operCalled){
                                emit operResponse(OperOK,"Oper OK");
                                operCalled = 0;
                            }
                            break;
                        case 401: 
                            if(privmsgCalled){
                                emit messageResponse(MessageNoSuchNick,"PRIVMSG No Such Nick");
                                privmsgCalled =0;
                            }
                            if(inviteCalled){
                                emit inviteResponse(InviteNoSuchNick,"Invite No Such Nick");
                                inviteCalled = 0;
                            }
                            if(whoisCalled){
                                emit whoisResponse(WhoisNoSuchNick,"Whois No Such Nick",who);
                                whoisCalled =0;
                            }
                            break;
                        case 403: 
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg3 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg4 = new QString(argRegExp.cap(1));
                            }
                            if(joinCalled){
                                emit channelResponse(ChannelNoSuchChannel,"No Such Channel",empty << *arg4);
                                joinCalled =0;
                            }
                            if(kickCalled){
                                emit kickResponse(KickNoSuchChannel,"Kick No Such Channel");
                                kickCalled = 0;
                            }
                            break;
                        case 404: 
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg3 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg4 = new QString(argRegExp.cap(1));
                            }
                            emit messageResponse(CannotSendToChannel,QString("Cannot Send To Channel :%1").arg(*arg4));
                            break;
                        case 405:
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg3 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg4 = new QString(argRegExp.cap(1));
                            }
                            emit channelResponse(TooManyChannels,"Too Many Channels",empty << *arg4);
                            break;
                        case 407: 
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg3 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg4 = new QString(argRegExp.cap(1));
                            }
                            emit channelResponse(TooManyTargets,"Too Many Channels",empty << *arg4);
                            break;
                        case 411: 
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg3 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            restLine->remove(0,1);
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg3 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg4 = new QString(argRegExp.cap(1));
                            }
                            emit messageResponse(NoRecipient,QString("No Recipient :%1").arg(*arg4));
                            break;
                        case 412:
                            emit messageResponse(NoTextToSend,"No Text To Send");
                            break;
                        case 413:
                            emit messageResponse(NoTopLevel,"No Top Level");
                            break;
                        case 414:
                            emit messageResponse(WildTopLevel,"Wild Top Level");
                            break;
                        case 431:
                            if(whoisCalled){
                                emit whoisResponse(WhoisNoNickGiven,"Whois No Nick Given",who);
                                whoisCalled =0;
                            }
                            if(nickCalled){
                                emit nickResponse(NickNoNickGiven,"Nick No Nick Given");
                                nickCalled =0;
                            }
                            break;
                        case 432: 
                            emit nickResponse(ErroneusNick,"Erroneus Nick");
                            break;
                        case 433: 
                            emit nickResponse(NickInUse,"Nick in Use");
                            break;
                        case 436: 
                            emit nickResponse(NickCollision,"Nick Collision");
                            break;
                        case 437:
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg3 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg4 = new QString(argRegExp.cap(1));
                            }
                            if(arg4->startsWith("#")){
                                pos = restRegExp.indexIn(*restLine);
                                if(pos>-1){
                                    restLine = new QString(restRegExp.cap(1));
                                }
                                pos = argRegExp.indexIn(*restLine);
                                if(pos>-1){
                                    arg3 = new QString(argRegExp.cap(1));
                                }
                                pos = restRegExp.indexIn(*restLine);
                                if(pos>-1){
                                    restLine = new QString(restRegExp.cap(1));
                                }
                                pos = argRegExp.indexIn(*restLine);
                                if(pos>-1){
                                    arg4 = new QString(argRegExp.cap(1));
                                }
                                emit channelResponse(ChannelUnavailResource,"Channel Unavailable Resource",empty << *arg4);
                            }
                            else
                                emit nickResponse(NickUnavailResource,"Nick Unavailable Resource");
                            break;
                        case 441:
                            if(kickCalled){
                                emit kickResponse(KickNotInChannel,"Kick Not In Channel");
                                kickCalled = 0;
                            }
                            break;
                        case 442:
//                             qDebug() << *currentLine;
                            if(partCalled){
                                emit partResponse(PartNotOnChannel,"Part Not On Channel");
                                partCalled = 0;
                            }
                            if(kickCalled){
                                emit kickResponse(KickNotOnChannel,"Kick Not On Channel");
                                kickCalled = 0;
                            }
                            if(inviteCalled){
                                emit inviteResponse(InviteNotOnChannel,"Invite Not On Channel");
                                inviteCalled =0;
                            }
                            if(topicCalled){
                                emit topicResponse(TopicNotOnChannel,"Topic Not On Channel");
                                topicCalled =0;
                            }
                            break;
                        case 443:
                            if(inviteCalled){
                                emit inviteResponse(InviteUserOnChannel,"Invite User On Channel");
                                inviteCalled = 0;
                            }
                            break;
                        case 461: 
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg3 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg4 = new QString(argRegExp.cap(1));
                            }
                            if(arg4->compare("USER")==0)
                                emit userResponse(UserNeedMoreParams,"User Need More Params");
                            if(arg4->compare("JOIN")==0){
                                pos = restRegExp.indexIn(*restLine);
                                if(pos>-1){
                                    restLine = new QString(restRegExp.cap(1));
                                }
                                pos = argRegExp.indexIn(*restLine);
                                if(pos>-1){
                                    arg3 = new QString(argRegExp.cap(1));
                                }
                                pos = restRegExp.indexIn(*restLine);
                                if(pos>-1){
                                    restLine = new QString(restRegExp.cap(1));
                                }
                                pos = argRegExp.indexIn(*restLine);
                                if(pos>-1){
                                    arg4 = new QString(argRegExp.cap(1));
                                }
                                emit channelResponse(ChannelNeedMoreParams,"Channel Need More Params",empty << *arg4);
                            }
                            if(arg4->compare("PART")==0)
                                emit partResponse(PartNeedMoreParams,"Part Need More Params");
                            if(arg4->compare("INVITE")==0)
                                emit inviteResponse(InviteNeedMoreParams,"Invite Need More Params");
                            if(arg4->compare("KICK")==0)
                                emit kickResponse(KickNeedMoreParams,"Kick Need More Params");
                            if(arg4->compare("OPER")==0)
                                emit operResponse(OperNeedMoreParams,"Oper Need More Params");
                            if(arg4->compare("TOPIC")==0)
                                emit topicResponse(TopicNeedMoreParams,"Topic Need More Params");
                            break;
                        case 462: 
                            emit userResponse(UserAlreadyRegistered,"User already registered");
                            break;
                        case 464:
                            if(operCalled){
                                emit operResponse(OperPasswordMismatch,"Oper Password Mismatch");
                                operCalled = 0;
                            }
                            break;
                        case 471: 
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg3 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg4 = new QString(argRegExp.cap(1));
                            }
                            emit channelResponse(ChannelIsFull,"Channel Is Full",empty << *arg4);
                            break;
                        case 473: 
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg3 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg4 = new QString(argRegExp.cap(1));
                            }
                            emit channelResponse(InviteOnlyChannel,"Invite Only Channel",empty << *arg4);
                            break;
                        case 474: 
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg3 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg4 = new QString(argRegExp.cap(1));
                            }
                            emit channelResponse(BannedFromChannel,"Banned From Channel",empty << *arg4);
                            break;
                        case 475: 
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg3 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg4 = new QString(argRegExp.cap(1));
                            }
                            emit channelResponse(BadChannelKey,"Bad Channel Key",empty << *arg4);
                            break;
                        case 476: 
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg3 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg4 = new QString(argRegExp.cap(1));
                            }
                            if(joinCalled){
                                emit channelResponse(ChannelBadChannelMask,"Bad Channel Mask",empty << *arg4);
                                joinCalled = 0;
                            }
                            if(kickCalled){
                                emit kickResponse(KickBadChannelMask,"Kick Bad Channel Mask");
                                joinCalled = 0;
                            }
                            break;
                        case 477: 
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg3 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg4 = new QString(argRegExp.cap(1));
                            }
                            if(joinCalled){
                                emit channelResponse(ChannelNoChannelModes,"Channel No Channel Modes",empty << *arg4);
                                joinCalled = 0;
                            }
                            if(topicCalled){
                                emit topicResponse(TopicNoChannelModes,"Topic No Channel Modes");
                                topicCalled = 0;
                            }
                            break;
                        case 478: 
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg3 = new QString(argRegExp.cap(1));
                            }
                            pos = restRegExp.indexIn(*restLine);
                            if(pos>-1){
                                restLine = new QString(restRegExp.cap(1));
                            }
                            pos = argRegExp.indexIn(*restLine);
                            if(pos>-1){
                                arg4 = new QString(argRegExp.cap(1));
                            }
                            emit channelResponse(BannedFromChannel,"Banned From Channel",empty << *arg4);
                            break;
                        case 482:
                            if(inviteCalled){
                                emit inviteResponse(InviteChannelOpNeeded,"Invite Channel Op Needed");
                                inviteCalled = 0;
                            }
                            if(kickCalled){
                                emit kickResponse(KickChannelOpNeeded,"Kick Channel Op Needed");
                                kickCalled = 0;
                            }
                            if(topicCalled){
                                emit topicResponse(TopicChannelOpNeeded,"Topic Channel Op Needed");
                                topicCalled = 0;
                            }
                            break;
                        case 484:
                            emit nickResponse(NickRestricted,"Nick Restricted");
                            break;
                        case 491:
                            emit operResponse(NoOperHost,"No Oper Host");
                            break;
                        default: 
                            break;
                    }
                }
                else{
                    // Handle Generic Response
//                     QRegExp user("[a-z]+");
                    QString user = *arg1;
                    QString command = *arg2;
                    QString nick = arg1->left(arg1->indexOf('!'));
//                     qDebug() << nick;
                    if( command == "PRIVMSG"){
                        emit genericResponse(PRIVMSG,nick,getArg3(currentLine),getRestLine3(currentLine),"");
                    }
                    if( command == "JOIN"){
                        emit genericResponse(JOIN,nick,getRestLine2(currentLine),"","");
                    }
                    if( command == "QUIT"){
                        emit genericResponse(QUIT,nick,getRestLine2(currentLine),"","");
                    }
                    if(command == "PART"){
                        emit genericResponse(PART,nick,getArg3(currentLine),getRestLine3(currentLine),"");
                    }
                    if(command == "KICK"){
                        emit genericResponse(KICK,nick,getArg3(currentLine),getRestLine4(currentLine),getArg4(currentLine));
                    }
                    if(command == "NICK"){
                        emit genericResponse(NICK,nick,getArg3(currentLine),getRestLine3(currentLine),"");
                    }
                }
        }
        else{
//             qDebug() << "YAY!";
//             std::cout << "yay!\n";
            QRegExp argRegExp("([^\\s]*)[\\s].*");
            int pos = argRegExp.indexIn(*currentLine);
            if(pos>-1){
                arg1 = new QString(argRegExp.cap(1));
            }
            QString *restLine;
            QRegExp restRegExp("[^\\s]*[\\s](.*)");
            pos = restRegExp.indexIn(*currentLine);
            if(pos>-1){
                restLine = new QString(restRegExp.cap(1));
            }

            if(arg1->compare("PING")==0){
                pos = argRegExp.indexIn(*restLine);
                if(pos>-1){
                    arg2 = new QString(argRegExp.cap(1));
                }
                arg2->remove(0,1);
                service->write(QString("PONG %1").arg(*arg2).toAscii());
            }
            else if(arg1->compare("NOTICE")==0){
                pos = argRegExp.indexIn(*restLine);
                if(pos>-1){
                    arg2 = new QString(argRegExp.cap(1));
                }
                pos = restRegExp.indexIn(*restLine);
                if(pos>-1){
                    restLine = new QString(restRegExp.cap(1));
                }
                restLine->remove(0,1);
                restLine->remove("\r\n");
//                 qDebug("Yay!");
                qDebug() << QString("%1 %2 :%3").arg(*arg1).arg(*arg2).arg(*restLine);
            }
        }
        i++;
    } // End While
}

void IrcData::errorHandler(QAbstractSocket::SocketError err)
{
    switch(err){
        case QAbstractSocket::ConnectionRefusedError:
            emit(connectResponse(ConnectionRefusedError,"ConnectionRefusedError"));
            break;
        case QAbstractSocket::RemoteHostClosedError:
            emit(connectResponse(RemoteHostClosedError,"RemoteHostClosedError"));
            break;
        case QAbstractSocket::HostNotFoundError:
            emit(connectResponse(HostNotFoundError,"HostNotFoundError"));
            break;
        case QAbstractSocket::SocketAccessError:
            emit(connectResponse(SocketAccessError,"SocketAccessError"));
            break;
        case QAbstractSocket::SocketResourceError:
            emit(connectResponse(SocketResourceError,"SocketResourceError"));
            break;
        case QAbstractSocket::SocketTimeoutError:
            emit(connectResponse(SocketTimeoutError,"SocketTimeoutError"));
            break;
        case QAbstractSocket::DatagramTooLargeError:
            emit(connectResponse(DatagramTooLargeError,"DatagramTooLargeError"));
            break;
        case QAbstractSocket::NetworkError:
            emit(connectResponse(NetworkError,"NetworkError"));
            break;
        case QAbstractSocket::AddressInUseError:
            emit(connectResponse(AddressInUseError,"AddressInUseError"));
            break;
        case QAbstractSocket::SocketAddressNotAvailableError:
            emit(connectResponse(SocketAddressNotAvailableError,"SocketAddressNotAvailableError"));
            break;
        case QAbstractSocket::UnsupportedSocketOperationError:
            emit(connectResponse(UnsupportedSocketOperationError,"UnsupportedSocketOperationError"));
            break;
        case QAbstractSocket::ProxyAuthenticationRequiredError:
            emit(connectResponse(ProxyAuthenticationRequiredError,"ProxyAuthenticationRequiredError"));
            break;
        case QAbstractSocket::SslHandshakeFailedError: 
            emit(connectResponse(SslHandshakeFailedError,"SslHandshakeFailedError"));
            break;
        case QAbstractSocket::UnknownSocketError: 
            emit(connectResponse(UnknownSocketError,"UnknownSocketError"));
            break;
        case QAbstractSocket::UnfinishedSocketOperationError:
            emit(connectResponse(UnfinishedSocketOperationError,"UnfinishedSocketOperationError"));
            break;
        default: 
            break;
    }
}

void IrcData::setConnected()
{
    emit(connectResponse(ConnectOK,"Connect OK"));
    Connected = 1;
}

bool IrcData::isConnected()
{
    return Connected;
}


