/////////////////////////////////////maks/////////////////////////
#include "sociotwitterwidget.h"
#include "socialqdbusplugindata.h"
//#include "tweetsupplier.h"
#include <pluginloader.h>
#include <QLineEdit>
#include <QPushButton>
#include<QScrollBar>
#include <QLabel>
#include<QFont>
#include<QImage>
#include<QMenu>
#include<QTimer>
#include <QWidget>
#include <QByteArray>
#include <QDBusArgument>
#include <QtDBus/QtDBus>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <QTest>
#include <QPalette>
#include <QCheckBox>
#include <QRegExp>

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
    argument >> status.status_id >> status.screen_name >> status.twit_text >> status.avatar_path >>status.twit_error_msg >> status.twit_is_error;// >> status.img;
    argument.endStructure();
    return argument;
}

SocioTwitter::SocioTwitter(QObject * object)
{
    base = new QWidget();
    base->resize(320,428);
    base->setBackgroundRole(QPalette::NoRole);
    base->setPalette(QColor(0,0,0,0.5));
    authenticated = false; // initialize the user authentication to false
    for (int i=0;i<3;i++)
    {
        parentLabelList.append(this->makeLabel(0,0,320,1100,1.0));
        scrollAreaList.append(this->makeQScrollArea(40,0,316,340,false,true,parentLabelList.at(i),base));
        scrollAreaList.at(i)->hide();
    }
    for (int i =0;i<240;i++)
    {
        tweetHolder.append(new QLabel());
        if (i<80)
        {
            tweetHolder[i]->setParent(parentLabelList[0]);
        }
        else if (i<160)
        {
            tweetHolder[i]->setParent(parentLabelList[1]);
        }
        else if (i<240)
        {
            tweetHolder[i]->setParent(parentLabelList[2]);
        }
    }
    twitPostEdit = new QLineEdit(base);
    twitPostEdit->move(0,385);
    twitPostEdit->resize(320,40);
    twitPostEdit->show();
    twitPostEdit->setStyleSheet("QLineEdit{border: 2px solid gray;border-radius: 5px;padding: 0 8px;}");
    logoff = new QPushButton(base);
    logoff->move(235,5);
    logoff->resize(75,35);
    logoff->setText("Log out");
    logoff->setStyleSheet("QPushButton{border: 2px solid gray;border-radius: 5px;padding: 0 8px;background-color:rgb(6,217,220); color:red;}");
    FTL = new QPushButton(base);
    FTL->move(0,5);
    FTL->resize(75,35);
    FTL->setText("Friends");
    FTL->setStyleSheet("QPushButton{border: 2px solid gray;border-radius: 5px;padding: 0 8px; background-color:rgb(255,255,255); color:red;}");
    FTL->setWindowOpacity(0.5);
    FTL->hide();

    MTL = new QPushButton(base);
    MTL->move(132,5);
    MTL->resize(95,35);
    MTL->setText("Mentions");
    MTL->setStyleSheet("QPushButton{border: 2px solid gray;border-radius: 5px;padding: 0 8px;background-color:rgb(255,255,255); color:red;}");
    MTL->hide();

    UTL = new QPushButton(base);
    UTL->move(76,5);
    UTL->resize(55,35);
    UTL->setText("User");
    UTL->setStyleSheet("QPushButton{border: 2px solid gray;border-radius: 5px;padding: 0 8px; background-color:rgb(255,255,255);color:red;}");
    UTL->hide();

    timer = new QTimer();
    //timer->start(120000);
    current_TL = 1;
    this->displayed = false;

    //SIGNAL - SLOT Object connectors
    connect(timer,SIGNAL(timeout()),this,SLOT(update()));
    connect(logoff,SIGNAL(clicked()),this,SLOT(log_off()));
    connect (twitPostEdit, SIGNAL(returnPressed ()) , this , SLOT(updateStatus()));
    
    connect(FTL,SIGNAL(clicked()),this,SLOT(showFTL()));
    connect(MTL,SIGNAL(clicked()),this,SLOT(showMTL()));
    connect(UTL,SIGNAL(clicked()),this,SLOT(showUTL()));
    //chb = new QCheckBox("Remember me",base);
    //chb->setStyleSheet("QCheckBox{color:white}");
    //chb->hide();
    //check authentication
    this->authenticated = false;
    if(authenticateMe())
    {
        this->authenticated = true;
        this->changePluginMethod("getFriendsTimeline");
        this->update();
        this->changePluginMethod("getUserTimeline");
        this->update();
        this->changePluginMethod("getUserNameMentions");
        this->update();
        this->closeDisplay();
        this->showTweets("FTL");
    }
    else
    {
        this->authenticated = false;
        this->displayLoginScreen();
    }
    INIT = true;
    l_bound = 0;
    //h_bound = 80;
    widget =  new PlexyDesk::SocioTwitterWidget(QRectF(0, 0, 360,480), base);
    base->move(20,20);
    DISP_FLAG = 0;

    //if (!authenticated)
    //else
      //  this->update();

}
SocioTwitter::~SocioTwitter(){}

void SocioTwitter::update()
{
    QVariantMap publications;
    QVariantMap credentials;
    if (authenticated)
    {
        credentials["uname"] =this->uname;
        credentials["pwd"] = this->pwd;
    }
    base->setEnabled(false);
    int try_update = 0 ;
    while (publications.isEmpty())
    {
        try_update++;
        if (try_update == 5)
        {
            publications["Network Error"] = QVariant("network error");
            break;
        }
        publications = SocialQDBusPluginData::getData("twitterplugin", pluginMethod , credentials);
    }
    //unwrapping the QVariantMap
    QStringList statusIds = publications.keys();
    //sort the ids
    QList<qlonglong> idList;
    for(int i=0; i<statusIds.length(); i++){
        idList<< (statusIds.at(i)).toLongLong();
    }
    qSort(idList.begin(), idList.end(), qGreater<qlonglong>());
    //end of sorting

    QStringList screenNames, tweets, avatars;
    if (statusIds.size() != 0)
    {
        if (statusIds[0] != "Network Error")
        {
            for (int i=0; i<statusIds.length(); i++)
            {
                QVariant firstVariant = publications[QString::number(idList.at(i))];
                QDBusArgument argu = qvariant_cast<QDBusArgument>(firstVariant);
                TwitData myDat = qdbus_cast<TwitData>(argu);
                screenNames<<myDat.screen_name;
                tweets << myDat.twit_text;
                avatars << myDat.avatar_path;
                //qDebug() << "SocioTwitterWidget: " << myDat.avatar_path;

            }
        }else{
            for (int i = 0;i<20;i++)
            {
                screenNames<<"Network Error";
                tweets<<"network error";
            }
        }
    }else
    {
        for (int i = 0;i<20;i++)
            {
                screenNames<<"Internal Error";
                tweets<<"Internal error";
            }
    }
    //unwrapping completed..

    //storing begin
    TimeLine[pluginMethod+"_sn"] = QVariant(screenNames);
    TimeLine[pluginMethod+"_tw"] = QVariant(tweets);
    TimeLine[pluginMethod+"_ap"] = QVariant(avatars);
    //edn storing
}
void SocioTwitter::showTweets(QString timeline)
{

    //retrieving data form the storage
    QStringList screenNames, tweets, avatars;
    int QSW_num;
    if (timeline == "FTL"){
        l_bound = 0;
        QSW_num = 0;
        screenNames = TimeLine["getFriendsTimeline_sn"].value<QStringList>();
        tweets = TimeLine["getFriendsTimeline_tw"].value<QStringList>();
        avatars = TimeLine["getFriendsTimeline_ap"].value<QStringList>();
    }else if (timeline == "MTL"){
        l_bound = 160;
        QSW_num = 2;
        screenNames = TimeLine["getUserNameMentions_sn"].value<QStringList>();
        tweets = TimeLine["getUserNameMentions_tw"].value<QStringList>();
        avatars = TimeLine["getUserNameMentions_ap"].value<QStringList>();
    }else if (timeline == "UTL"){
        l_bound = 80;
        QSW_num = 1;
        screenNames = TimeLine["getUserTimeline_sn"].value<QStringList>();
        tweets = TimeLine["getUserTimeline_tw"].value<QStringList>();
        avatars = TimeLine["getUserTimeline_ap"].value<QStringList>();
    }
    //Data retrieving complete

    //setting up the display
    FTL->show();
    MTL->show();
    UTL->show();
    tweetCount=0;
    int set= screenNames.size();
    if (set > tweets.size())
        set = tweets.size();
    if (set > avatars.size())
        set = avatars.size();
    QLabel *t = (QLabel *)(tweetHolder[l_bound]->parentWidget());
    t->resize(320,set*65);
    t->show();
    scrollAreaList.at(QSW_num)->show();
    for (int i=0;i<set*4;)
    {
        tweetHolder[l_bound+i]->move(-11,(i/4)*65);//2
        tweetHolder[l_bound+i]->resize(280,65);//(290,55)

        //setting up the screenName Display{
        tweetHolder[l_bound+i+1]->move(44,2+(i/4)*65);//move(44,2+(i/4)*55);
        tweetHolder[l_bound+i+1]->resize(238,12);//(247,12)
        QString temp1 = QString("<font size=1 color=white>");
        temp1.append(screenNames[tweetCount]);
        temp1.append("</font>");
        tweetHolder[l_bound+i+1]->setText(temp1);
        //screenName displaying Configured}

        //setting up tweet display{
        tweetHolder[l_bound+i+2]->move(44,14+(i/4)*65);
        tweetHolder[l_bound+i+2]->resize(238,55);//(238,45);
        tweetHolder[l_bound+i+2]->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
        tweetHolder[l_bound+i+2]->setOpenExternalLinks(true);
        tweetHolder[l_bound+i+2]->setWordWrap(true);
        temp1 = QString("<font size=1 color=cyan>");
        //setting up the regX for links in tweets
        QString tw = tweets[tweetCount];

        //QRegExp l_RX = QRegExp("http://",Qt::CaseInsensitive,QRegExp::RegExp);
        //int startInd = tw.indexOf(l_RX,0);
        int startInd = tw.indexOf("http://",0,Qt::CaseInsensitive);
        if (startInd != -1)
        {
            tw.remove(0,startInd);
            //qDebug()<<"---"<<tw;
            int endInd = tw.indexOf(" ",0) ;
            if (endInd == -1)
            {
                endInd = tw.length();
            }
            //qDebug()<<endInd<<"start index"<<startInd;
            tw.truncate(endInd);
            QString linkedTweet = tweets[tweetCount];
            if (tw.endsWith("/",Qt::CaseInsensitive))
                tw.truncate(tw.length()-1);
            linkedTweet.insert(startInd,"<a href="+tw+">");
            int t_Len = tw.length();
            linkedTweet.insert(startInd+2*t_Len+9,"</a>");
            //qDebug()<<"linked text"<<linkedTweet;
            temp1.append(linkedTweet);
        }else
            temp1.append(tweets[tweetCount]);

        //qDebug()<<startInd<<" "<<screenNames[tweetCount]<<tw;
        //Completed regX
        temp1.append("</font>");
        //tweetHolder[l_bound+i+2]->setTextFormat(Qt::RichText);
        tweetHolder[l_bound+i+2]->setText(temp1);
        tweetHolder[l_bound+i+2]->setTextFormat(Qt::RichText);
        tweetHolder[l_bound+i+2]->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        //tweet diplay configured}

        //setting up the avatar{
        tweetHolder[l_bound+i+3]->move(3,10+(i/4)*65);
        tweetHolder[l_bound+i+3]->setPixmap(QPixmap::fromImage(QImage(avatars[tweetCount])));
        tweetHolder[l_bound+i+3]->resize(39,39);
        tweetHolder[l_bound+i+3]->show();
        //avatar configurations complete}

        i = i+4;
        tweetCount++;
    }
    base->setEnabled(true);
    tweetCount = 0;
}
bool SocioTwitter::isLogged()
{
    return false;
}
void SocioTwitter::changePluginMethod(QString methodName)
{
    pluginMethod = methodName;
}

void SocioTwitter::log_off()
{
    QVariantMap args, logStatus;
    logStatus = SocialQDBusPluginData::getData("twitterplugin", "endSession", args);
    QVariant onlyVariant = logStatus["feedback"];
    QDBusArgument argu = qvariant_cast<QDBusArgument>(onlyVariant);
    TwitData status = qdbus_cast<TwitData>(argu);

    if (!status.twit_is_error) {
        qDebug()<<"Yay Logout works just fine *****************";
    authenticated = false;
    timer->stop();
    TimeLine.clear();
    FTL->hide();
    MTL->hide();
    UTL->hide();
    scrollAreaList.at(0)->hide();
    scrollAreaList.at(1)->hide();
    scrollAreaList.at(2)->hide();
    this->displayLoginScreen();
    }
    else {
        //logoff unsuccessful
    }
}

void SocioTwitter::displayLoginScreen()
{
    logoff->hide();
    displayed = true;
    usernameLabel = new QLabel("<font size=2 color=red>Username : </font>",base);
    passwordLabel = new QLabel("<font size=2 color=red>Password  : </font>",base);
    loginStatus = new QLabel(base);
    usernameEdit = new QLineEdit(base);
    passwordEdit = new QLineEdit(base);
    submit = new QPushButton("Submit",base);
    submit->setStyleSheet("QPushButton{border: 2px solid gray;border-radius: 5px;padding: 0 8px; color:white;}");
    usernameLabel->move(5,203);
    usernameEdit->move(80,200);
    usernameEdit->resize(200,22);
    usernameEdit->hide();
    usernameLabel->setBuddy(usernameEdit);
    usernameEdit->setStyleSheet("QLineEdit{border: 2px solid gray;border-radius: 5px;padding: 0 8px;}");
    submit->move(130,260);
    submit->resize(150,20);
    passwordLabel->move(5,233);
    passwordLabel->hide();
    passwordEdit->move(80,230);
    passwordEdit->resize(200,22);
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->hide();
    passwordEdit->setStyleSheet("QLineEdit{border: 2px solid gray;border-radius: 5px;padding: 0 8px;}");
    passwordLabel->setBuddy(passwordEdit);
    loginStatus->move(5,200);
    loginStatus->setTextFormat(Qt::RichText);
    twitPostEdit->hide();
    usernameLabel->show();
    usernameEdit->show();
    passwordLabel->show();
    passwordEdit->show();
    submit->show();
    submit->setEnabled(true);
    //chb = new QCheckBox("Remember me",base);
    //chb->setStyleSheet("QCheckBox{color:white}");
    //chb->setPalette(QPalette(0,0,0,0.5));
    //chb->move(20,280);
    //disabling the remember me functionality;
    //chb->show();
    connect(submit,SIGNAL(clicked()),this,SLOT(isAuthenticated()));
}
void SocioTwitter::isAuthenticated()
{
    timer->stop();
    if((this->usernameEdit->text()).isEmpty() || (this->passwordEdit->text().isEmpty()))
    {
        return;
    }

    this->authenticated = true;
    this->uname = this->usernameEdit->text();
    this->pwd = this->passwordEdit->text();
    this->changePluginMethod("getFriendsTimeline");
    this->update();
    this->changePluginMethod("getUserTimeline");
    this->update();
    this->changePluginMethod("getUserNameMentions");
    this->update();
    this->closeDisplay();
    this->showTweets("FTL");
}
void SocioTwitter::closeDisplay()
{
    DISP_FLAG = 2;
    displayed = false;
    twitPostEdit->show();
    //tabWidget->resize(320,380);
    usernameLabel->hide();
    usernameEdit->hide();
    passwordLabel->hide();
    passwordEdit->hide();
    //chb->hide();
    submit->hide();
    loginStatus->hide();
    this->refresh();
}
void SocioTwitter::refresh()
{
    if (!authenticated)
    {
        logoff->hide();
    }
    else
    {
        logoff->show();
    }
}
bool SocioTwitter::authenticateMe()
{
    QSettings settings("plexydesk/social/daemon/crypt", "dat");
    this->uname = settings.value("credentials/uname").toString();
    this->pwd = settings.value("credentials/pwd").toString();
    //thesting
    qDebug() << this->uname;

    if(this->uname.isEmpty() || this->pwd.isEmpty())
        return false;
    return true;
}
void SocioTwitter::updateStatus()
{
    qDebug()<<"SocioTwitter::updateStatus() called";
    if(this->twitPostEdit->text().isEmpty())
        return;
    if(!this->authenticated){
        this->displayLoginScreen();
        return;
    }
    QString newStatus = twitPostEdit->text();
    QVariantMap args;
    args["uname"] = this->uname;
    args["pwd"] = this->pwd;
    args["status"] = newStatus;

   /* parentLabelList.at(0)->hide();
    parentLabelList.at(1)->hide();
    parentLabelList.at(2)->hide();*/
    base->setUpdatesEnabled(false);


    SocialQDBusPluginData::getData("twitterplugin", "updateStatus", args);
    //refresh elements

   /*  parentLabelList.at(0)->show();
    parentLabelList.at(1)->show();
    parentLabelList.at(2)->show();*/

    this->changePluginMethod("getFriendsTimeline");
    this->update();


     base->setUpdatesEnabled(true);
     qDebug()<<current_TL;
     if (current_TL == 1)
        showTweets("FTL");
     else if (current_TL == 2)
        showTweets("UTL");
     else if (current_TL == 3)
        showTweets("MTL");


    qDebug() << "end of SocioTwitter::updateStatus()";
}
void SocioTwitter::showFTL()
{
    if (!parentLabelList.at(0)->isHidden())
        parentLabelList.at(0)->hide();
    if (!parentLabelList.at(1)->isHidden())
        parentLabelList.at(1)->hide();
    if (!parentLabelList.at(2)->isHidden())
        parentLabelList.at(2)->hide();
    if (!scrollAreaList.at(0)->isHidden())
        scrollAreaList.at(0)->hide();
    if (!scrollAreaList.at(1)->isHidden())
        scrollAreaList.at(1)->hide();
    if (!scrollAreaList.at(2)->isHidden())
        scrollAreaList.at(2)->hide();
    current_TL = 1;
    this->showTweets("FTL");
}
void SocioTwitter::showMTL()
{
    if (!parentLabelList.at(0)->isHidden())
        parentLabelList.at(0)->hide();
    if (!parentLabelList.at(1)->isHidden())
        parentLabelList.at(1)->hide();
    if (!parentLabelList.at(2)->isHidden())
        parentLabelList.at(2)->hide();
    if (!scrollAreaList.at(0)->isHidden())
        scrollAreaList.at(0)->hide();
    if (!scrollAreaList.at(1)->isHidden())
        scrollAreaList.at(1)->hide();
    if (!scrollAreaList.at(2)->isHidden())
        scrollAreaList.at(2)->hide();
    current_TL = 3;
    this->showTweets("MTL");
}
void SocioTwitter::showUTL()
{
    if (!parentLabelList.at(0)->isHidden())
        parentLabelList.at(0)->hide();
    if (!parentLabelList.at(1)->isHidden())
        parentLabelList.at(1)->hide();
    if (!parentLabelList.at(2)->isHidden())
        parentLabelList.at(2)->hide();
    if (!scrollAreaList.at(0)->isHidden())
        scrollAreaList.at(0)->hide();
    if (!scrollAreaList.at(1)->isHidden())
        scrollAreaList.at(1)->hide();
    if (!scrollAreaList.at(2)->isHidden())
        scrollAreaList.at(2)->hide();
    current_TL = 2;
    this->showTweets("UTL");
}
QString SocioTwitter::getScreenName(int index)
{
    //return tweetsupplier->getSpecificScreenName(index);
}
QGraphicsItem * SocioTwitter::item()
{
  return widget;
}
QLabel * SocioTwitter::makeLabel(int x,int y, int w, int h, qreal opacityLevel)
{
    QLabel * label = new QLabel;
    label->move(y,x);
    label->resize(w,h);
    label->setWindowOpacity(opacityLevel);
    return label;
}
QLabel * SocioTwitter::makeLabel(int x, int y, int w, int h, QWidget * parent)
{
    QLabel * label = new QLabel;
    label->move(y,x);
    label->resize(w,h);
    label->setParent(parent);
    return label;
}
QScrollArea * SocioTwitter::makeQScrollArea(int x, int y, int w, int h, bool HScrollBar,bool VScrollBar)
{
    QScrollArea * area = new QScrollArea;
    area->move(y,x);
    area->resize(w,h);
    if (HScrollBar && VScrollBar)
    {
        area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    }else if (!HScrollBar && VScrollBar) {
        area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    }else if (HScrollBar && !VScrollBar) {
        area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }else if (!HScrollBar && !VScrollBar) {
        area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    return area;
}
QScrollArea * SocioTwitter::makeQScrollArea(int x, int y, int w, int h, bool HScrollBar,bool VScrollBar,QWidget *widget,bool setAsParent,bool setAsWidget)
{
    QScrollArea * area = new QScrollArea;
    area->move(y,x);
    area->resize(w,h);
    if (HScrollBar && VScrollBar)
    {
        area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    }else if (!HScrollBar && VScrollBar) {
        area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    }else if (HScrollBar && !VScrollBar) {
        area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }else if (!HScrollBar && !VScrollBar) {
        area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    if (setAsWidget)
        area->setWidget(widget);
    if (setAsParent)
        area->setParent(widget);
    return area;
}
QScrollArea * SocioTwitter::makeQScrollArea(int x, int y, int w, int h, bool HScrollBar,bool VScrollBar,QWidget *widget,QWidget * parent)
{
    QScrollArea * area = new QScrollArea;
    area->move(y,x);
    area->resize(w,h);
    if (HScrollBar && VScrollBar)
    {
        area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    }else if (!HScrollBar && VScrollBar) {
        area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    }else if (HScrollBar && !VScrollBar) {
        area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }else if (!HScrollBar && !VScrollBar) {
        area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    area->setWidget(widget);
    area->setParent(parent);
    return area;
}
QScrollArea * SocioTwitter::makeQScrollArea(int x,int y, int w, int h)
{
    QScrollArea * area = new QScrollArea;
    area->move(y,x);
    area->resize(w,h);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    return area;
}


void SocioTwitter::rememberMeSet(bool state)
{
    if(state)
    {
        this->uname = "test";
        this->pwd = "testpwd";
        QSettings settings("plexydesk/social/daemon/crypt", "dat");
        QVariant user(this->uname);
        QVariant passwd(this->pwd);
        settings.setValue("credentials/uname", user);
        settings.setValue("credentials/pwd", passwd);
    }

}

