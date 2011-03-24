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
#ifndef PLEXY_CLASSIC_BACKDROP_H
#define PLEXY_CLASSIC_BACKDROP_H

#include <QPainter>
#include <QDesktopWidget>
#include <QPixmap>
#include <plexy.h>
#include <config.h>
#include <backdropplugin.h>


class VISIBLE_SYM BgPlugin :public PlexyDesk::BackdropPlugin
{
    Q_OBJECT

    public:
        BgPlugin(QObject * object = 0);
        virtual ~BgPlugin();
        void render(QPainter *p,QRectF r);
    public slots:
        void data(QVariant&);
        void pushData(QVariant&) {}
    signals:
        void dataChange();
    private:
        QPixmap bg;
        QImage img;
        QBrush paint;
 ////       PlexyDesk::DataInterface * flickrEngine;
    //    PlexyDesk::DataInterface *wallpaperWin;
        int width;
        int height;
};


#endif
