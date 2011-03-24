////------------------------maks----------
#ifndef PLEXY_WIDGET_TWITTER_H
#define PLEXY_WIDGET_TWITTER_H

#include <plexy.h>
#include <abstractplugininterface.h>
#include <datainterface.h>
#include <widgetinterface.h>
#include "sociotwitter.h"
//#include "tweetsupplier.h"
#include <widgetplugin.h>
#include <QScrollArea>
#include <QPixmap>
#include <QTabWidget>
#include <QTimer>
class QCheckBox;
class QPushButton;
class QLineEdit;
class QWidget;
class QLabel;
class VISIBLE_SYM SocioTwitter : public PlexyDesk::WidgetPlugin
{
    Q_OBJECT
    public:

        SocioTwitter(QObject * object = 0);
        virtual ~SocioTwitter();
        virtual QGraphicsItem * item();// {};
    public slots:
        void data(QVariant&);

    private slots:
        void showFTL();
        void showUTL();
        void showMTL();
        void log_off();
        void isAuthenticated();
        void closeDisplay();
        void update();
        void refresh();
        void updateStatus();
   signals:
        void change();
        void sendData(QVariant&);
   private:        
        QString getScreenName(int index);
        QWidget * base;
        QLineEdit * twitPostEdit,* usernameEdit, * passwordEdit;
        QList<QLabel *> parentLabelList;
        QList<QLabel *> tweetHolder;
        QList<QScrollArea *> scrollAreaList;        
        QLabel  * usernameLabel, * passwordLabel, * loginStatus;
        QPushButton  * logoff, * submit,*FTL,*UTL,*MTL;
        QVariantMap TimeLine;
        QCheckBox * chb;
        int current_TL;
        bool authenticated,displayed,INIT;
        bool isLogged();
        bool authenticateMe();
        void displayLoginScreen();
        void changePluginMethod(QString methodName);
        void showTweets(QString timeline);
	 void rememberMeSet(bool state);

        QTimer *timer;
        int tweetCount,l_bound,DISP_FLAG;
        QString pluginMethod, uname, pwd;
        QLabel * makeLabel(int x, int y, int w, int h, qreal opacityLevel);
        QLabel * makeLabel(int x, int y, int w, int h, QWidget *parent);
        QScrollArea * makeQScrollArea(int x, int y, int w, int h);
        QScrollArea * makeQScrollArea(int x, int y, int w, int h,bool HScrollBar,bool VScrollBar);
        QScrollArea * makeQScrollArea(int x, int y, int w, int h,bool HScrollBar,bool VScrollBar, QWidget *widget,bool setAsParent,bool setAsWidget);
        QScrollArea * makeQScrollArea(int x, int y, int w, int h,bool HScrollBar,bool VScrollBar, QWidget *widget,QWidget * parent);

        PlexyDesk::SocioTwitterWidget * widget;
};
#endif
