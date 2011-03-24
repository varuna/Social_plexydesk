/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@kde.org>
*  Authored By  :
*
*  PlexyDesk is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  PlexyDesk is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with PlexyDesk. If not, see <http://www.gnu.org/licenses/lgpl.html>
*******************************************************************************/

#include "videoplugin.h"

#include <QtCore>
#include <pluginloader.h>
#include <QtGui>

VideoPlugin::VideoPlugin(QObject * object)
{
	base = new QWidget();

	flow = new QLabel(base);
	flow->resize(300,170);
	flow->move(0,0);
	base->resize(300,200);

	search = new QLineEdit(base);
	search->setStyleSheet("border:1px solid ; font-style:strong ;padding-left:20px; background: black ; background-image:url(" + applicationDirPath() + "/share/plexy/skins/default/flick/bg-search.png) ;background-repeat: no-repeat ;  color:black ");
	search->move(0,170);
	search->resize(300,30);
	search->show();
	connect (search , SIGNAL(returnPressed ()) , this , SLOT(searchImage () ) );
	widget =  new PlexyDesk::VideoWidget(QRectF(0, 0, 340,240), base);

	base->move(20,20);
}

VideoPlugin::~VideoPlugin()
{
	delete base;
}

void VideoPlugin::searchImage ()
{
	qDebug() << "Searching video" << endl;
	search->setEnabled(false);

	QVariant data(search->text());
	emit sendData(data);
}


void VideoPlugin::data(QVariant& data)
{
	QImage wall = data.value<QImage>();
	search->setEnabled(true);
	flow->setPixmap(QPixmap::fromImage(wall));
}

QGraphicsItem * VideoPlugin::item()
{
	PlexyDesk::PluginLoader * loader = new PlexyDesk::PluginLoader();
	loader->scanDisk();
	videoEngine  = (PlexyDesk::DataInterface*) loader->instance("videoengine");

	if (videoEngine) {
		connect(videoEngine, SIGNAL(data(QVariant&)), this, SLOT(data(QVariant&)));
		connect(this, SIGNAL(sendData(QVariant&)), videoEngine, SLOT(pushData(QVariant&)));
	}else {
		qDebug() << "DataSource Was Null" << "VideoPlugin::VideoPlugin(QObject * object)" << endl;
	}

	return widget;
}

