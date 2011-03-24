#include "duckwidget.h"
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
#include "socialqdbusplugindata.h"

//QVariantMap  test;
struct DuckData
{
  
  QString some_text;
  QString some_url;

};

QDBusArgument &operator<<(QDBusArgument &argument, const DuckData &results)
{
        argument.beginStructure();
        argument << results.some_text << results.some_url;
        argument.endStructure();
        return argument;
}
const QDBusArgument &operator>>(const QDBusArgument &argument, DuckData &results)
{
        argument.beginStructure();
        argument >> results.some_text >> results.some_url;
        argument.endStructure();
        return argument;
}

Duck::Duck(QObject * object)
{
    base = new QWidget();
    base->resize(310,418);//small square
/*
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
*/
    widget =  new PlexyDesk::DuckWidget(QRectF(0, 0, 330,465), base);//outer square
    base->move(10,25);
    QVariantMap mp;
    mp["args"] = QVariant("argsfromWidget");
    SocialQDBusPluginData *tr = new SocialQDBusPluginData();
    QVariantMap p = tr->getData("duckplugin","searchResult",mp);
    qDebug()<<p;
}

void Duck::update()
{
    qDebug()<<"Duck::update() called";
    QVariantMap publications;
    //QVariantMap empty;
    //test["input"] = this->input;
 
    publications = SocialQDBusPluginData::getData("duckplugin", "sendSearch", test);

    //unwrapping the QVariantMap
    QStringList searchIds = publications.keys();

    //sort the ids
    QList<qlonglong> idList;
    for(int i=0; i<searchIds.length(); i++){
        idList<< (searchIds.at(i)).toLongLong();
    }
    qSort(idList.begin(), idList.end(), qGreater<qlonglong>());
    //end of sorting

    QStringList result_url, result_text;
    if (searchIds[0] != "Network Error")
    {     
        for (int i=0; i<searchIds.length(); i++)
        {
            QVariant firstVariant = publications[QString::number(idList.at(i))];

            QDBusArgument argu = qvariant_cast<QDBusArgument>(firstVariant);
            DuckData myDat = qdbus_cast<DuckData>(argu);
            result_url<<myDat.some_url;
            result_text << myDat.some_text;
            

        }     
    }else{     
        for (int i = 0;i<20;i++)
        {
            result_url<<"Network Error";
            result_text<<"network error";
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
         			temp1.append(result_url[i]);
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
         			temp1.append(result_text[i]);
         			temp1.append("</font>");
         			content->setText(temp1);
        j=j+76;
        			 	
     	}
}
Duck::~Duck()
{
    
}
QGraphicsItem * Duck::item()
{
   	return widget;
}
void Duck::sendSearch()
{
    qDebug()<<"Duck::sendSearch() is called";
    if(this->searchField->text().isEmpty())
	{
        	return;
        }else{ 
    		QString userInput = searchField->text();
    		QVariantMap args;
    		
		//test["input"] = userInput;

    	        args["input"] = userInput;
    		SocialQDBusPluginData::getData("duckplugin", "sendSearch", args);
    		this->update();
		
    		qDebug() << "end of Duck::sendSearch()";
	     }
}


