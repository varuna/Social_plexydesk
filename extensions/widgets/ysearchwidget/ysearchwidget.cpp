#include "ysearchwidget.h"
#include "socialqdbusplugindata.h"
#include <QUrl>
#include <pluginloader.h>
#include <QLineEdit>
#include <QPushButton>
#include<QScrollArea>
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

//QVariantMap  test;
struct YData
{
  
  QString title;
  QString summery;
  QString url;
  QString disp_url; 

};

QDBusArgument &operator<<(QDBusArgument &argument, const YData &results)
{
        argument.beginStructure();
        argument << results.title << results.summery << results.url << results.disp_url;
        argument.endStructure();
        return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, YData &results)
{
        argument.beginStructure();
        argument >> results.title >> results.summery >> results.url >> results.disp_url;
        argument.endStructure();
        return argument;
}



YSearch::YSearch(QObject * object)
{
    base = new QWidget();
    base->resize(310,418);//small square

    frontlabel = new QLabel(base);
    frontlabel->move(0,0);
    frontlabel->setStyleSheet("border:3px solid;border-color:#666666;background-color:black;opacity:10");     
    frontlabel->resize(310,418);
    frontlabel->show();
  
    frontbadge = new QLabel(frontlabel);
    frontbadge->setStyleSheet("border:0px;background-image:url(/usr/share/plexy/skins/default/duck/logo.png)");
    frontbadge->move(80,50);
    frontbadge->resize(150,150);
    frontbadge->show();
    //frontbadge->hide();
	
    bottomlabel = new QLabel(base);
    bottomlabel->move(0,0);
    bottomlabel->resize(310,418);
    bottomlabel->hide();
    //bottomlabel->show();   
	
    biglabel = new QLabel(bottomlabel);
    biglabel->setStyleSheet("border:3px solid;border-color:#666666;background-color:black");
    biglabel->move(0,0);
    biglabel->resize(310,418);
    biglabel->show();


    searchField = new QLineEdit(frontlabel);
    searchField->setStyleSheet("border:3px solid ; font-style:strong ;padding-right:5px;padding-left:5px; background:#CCCCCC;border-radius:4px");
    searchField->move(28,275);
    searchField->resize(250,30);
    searchField->show();

    sbutton = new QPushButton("Search", frontlabel);
    sbutton->setStyleSheet("background-color:#cccccc;border:0px;font:bold;");
    sbutton->move(122,312);
    sbutton->resize(60,24);
    sbutton->show();

    backbutton = new QPushButton("<-", bottomlabel);
    backbutton->setStyleSheet("background-color:#cccccc;border:0px;font:bold;");
    backbutton->move(122,380);
    backbutton->resize(60,24);
    backbutton->show();
    
    connect (sbutton ,SIGNAL(clicked()), bottomlabel,SLOT(show()));
    connect (backbutton ,SIGNAL(clicked()), bottomlabel,SLOT(hide()));	
    connect (sbutton , SIGNAL(clicked()),this,SLOT(sendSearch()));

    container = new QLabel(bottomlabel);
    container->move(4,40);
    container->resize(302,760);
    //container->setStyleSheet("border:1px solid;border-color:white");
    container->show();
    
     
     scroll = new QScrollArea;
     scroll->move(4,15);
     scroll->setStyleSheet("border:0px solid;border-color:black");
     scroll->resize(302,360);
     scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
     scroll->setWidget(container);
     scroll->setParent(bottomlabel);	
//*/
    widget =  new PlexyDesk::YSearchWidget(QRectF(0, 0, 330,465), base);//outer square
    base->move(10,25);
  /*  QVariantMap ar;
    ar["query"] = QVariant("plexydesk");
    QVariantMap t = SocialQDBusPluginData::getData("ysearchplugin","searchResult",ar);
    qDebug()<<t;*/
}

void YSearch::update()
{
    qDebug()<<"Duck::update() called";
    QVariantMap publications;
    QVariantMap empty;
    //test["input"] = this->input;
 
    publications = SocialQDBusPluginData::getData("ysearchplugin", "sendSearch", empty);

    //unwrapping the QVariantMap
    QStringList searchIds = publications.keys();

    //sort the ids
    QList<qlonglong> idList;
    for(int i=0; i<searchIds.length(); i++){
        idList<< (searchIds.at(i)).toLongLong();
    }
    qSort(idList.begin(), idList.end(), qGreater<qlonglong>());
    //end of sorting

    QStringList y_title,y_summery,y_url,y_dispUrl;
    if (searchIds[0] != "Network Error")
    {     
        for (int i=0; i<searchIds.length(); i++)
        {
            QVariant firstVariant = publications[QString::number(idList.at(i))];

            QDBusArgument argu = qvariant_cast<QDBusArgument>(firstVariant);
            YData myDat = qdbus_cast<YData>(argu);
            y_title << myDat.title;
	    y_summery <<myDat.summery;
	    y_url << myDat.url;
	    y_dispUrl << myDat.disp_url;

        }     
    }else{     
        for (int i = 0;i<20;i++)
        {
            y_title << "Network Error";
	    y_summery << "Network Error";
	    y_url << "Network Error";
	    y_dispUrl << "Network Error";
        }

    }
	
	int j=0;
        for(int i=0;i<10;i++)
	{
         resultpack[i] = new QLabel(bottomlabel);
         resultpack[i]->setParent(container);
    	
    	//result2->setStyleSheet("background-image:url(/usr/share/plexy/skins/default/duck/wall.png)");
    	resultpack[i]->setStyleSheet("border:1px solid;border-radius:4px;border-color:red");
    	resultpack[i]->move(0,j);
    	resultpack[i]->resize(280,72);
    	resultpack[i]->show();
		
			header = new QLabel(bottomlabel);
			header->setParent(resultpack[i]);
			header->setFrameStyle(QFrame::Box | QFrame::Raised);
         		header->setLineWidth(1);
        		header->setMidLineWidth(0);
			header->setStyleSheet("border:0px");		
			header->move(0,0);
			header->resize(250,25);
			header->show();	

				QString temp1 = QString("<font size=1 color=white>");
         			temp1.append(y_url[i]);
         			temp1.append("</font>");
         			header->setText(temp1);

			content = new QLabel(bottomlabel);
			content->setParent(resultpack[i]);
			content->setFrameStyle(QFrame::Box | QFrame::Raised);
	                content->setLineWidth(1);
                        content->setMidLineWidth(0);
			content->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  			content->setStyleSheet("border:0px");
			content->move(0,25);
			content->resize(278,47);
			content->show();		
				
			        temp1 = QString("<font size=1 color=white>");
         			temp1.append(y_summery[i]);
         			temp1.append("</font>");
         			content->setText(temp1);
        j=j+76;
        			 	
     	}
}

YSearch::~YSearch()
{
    
}
 
QGraphicsItem * YSearch::item()
{
   	return widget;
}


void YSearch::sendSearch()
{
    qDebug()<<"YSearch::sendSearch() is called";
    if(this->searchField->text().isEmpty())
	{
        	return;
        }else{ 
    		QString userInput = searchField->text();
    		QVariantMap args;
    		
		//test["input"] = userInput;

    	        args["input"] = userInput;
    		SocialQDBusPluginData::getData("ysearchplugin", "sendSearch", args);
    		this->update();
		
    		qDebug() << "end of YSearch::sendSearch()";
	     }
}


