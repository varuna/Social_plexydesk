
#ifndef PLEXY_WIDGET_YSEARCH_H
#define PLEXY_WIDGET_YSEARCH_H

#include <plexy.h>
#include <abstractplugininterface.h>
#include <datainterface.h>
#include <widgetinterface.h>
#include "ysearch.h"
#include <widgetplugin.h>

class QLineEdit;
class QWidget;
class QPushButton;
class QLabel;
class QScrollArea;

class VISIBLE_SYM YSearch : public PlexyDesk::WidgetPlugin
{
    Q_OBJECT

    public:
        YSearch(QObject * object = 0);
        virtual ~YSearch();
        virtual QGraphicsItem * item();// {};
   
   public slots:
        void data(QVariant&);
   //     void searchImage();

   private slots:
	void update();
	void sendSearch();
   
   signals:
   //     void change();
       void sendData(QVariant&);
   private:
        //PictureFlow * flow ;
        //void search();
	//void update();
        QWidget * base;
        QLineEdit * searchField;
	QPushButton * sbutton;
        QPushButton * backbutton;
	QLabel * biglabel;
	QLabel * logolabel;
        QLabel * container;
        QScrollArea * scroll;
	QLabel * resultpack[10];
	QLabel * header;   
	QLabel * content;
	QLabel * bottomlabel;
        QLabel * frontlabel;
        QLabel * frontbadge;
        
	QVariantMap test;
	     
        
	// PlexyDesk::DataInterface * flickrEngine;
	  
        PlexyDesk::YSearchWidget * widget;
};

#endif
