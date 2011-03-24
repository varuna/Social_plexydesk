/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "icon.h"
#include <qplexymime.h>
#include <iconprovider.h>
#include <QPainter>

namespace PlexyDesk
{

class Icon::Private
{
public:
    Private() {}
    ~Private() {}
    QPixmap icon;
    QPlexyMime mime;
    QString text;
    bool valid;
    IconProviderPtr iconprovider;
    uint id;
    IconJobPtr iconjob;
};

Icon::Icon(IconProviderPtr icon, const QRectF &rect, QWidget *embeddedWidget) : DesktopWidget(rect), d(new Private)
{
    d->valid = false;
    d->iconprovider = icon;
    d->id = 0;
}


Icon::~Icon()
{
    delete d;
}

void Icon::loadIcon()
{
    d->icon = d->iconjob->Icon();
    update();
}

void Icon::setContent(const QString& path)
{
    d->mime.fromFileName(path);
    QString iconname;
    if (d->mime.mimeType() == "application/x-desktop")
    {
        QSettings setting(path,  QSettings::IniFormat);
        setting.beginGroup("Desktop Entry");
        iconname = setting.value("Icon","").toString();
        d->text = setting.value("Name","").toString();
        setting.endGroup();
        d->valid = true;
        d->iconjob = d->iconprovider->requestIcon(iconname, "32x32");
        connect(d->iconjob.data(), SIGNAL(finished()), this, SLOT(loadIcon()));
    } else {
        QString name = d->mime.genericIconName();
    }

}

bool Icon::isValid()
{
    return d->valid;
}

void Icon::paintBackSide(QPainter * painter,const QRectF& rect)
{
    if (!d->icon.isNull()) {
        int x = (this->boundingRect().width() - d->icon.width())/2;
        painter->drawPixmap(QRect(x, x , d->icon.width(), d->icon.height()) , d->icon);
    }
    drawText(painter, rect);
    // DesktopWidget::paintDockView(painter, rect);
}
void Icon::paintViewSide(QPainter * painter,const QRectF& rect)
{
    if (!d->icon.isNull()) {
        int x = (this->boundingRect().width() - d->icon.width())/2;
        painter->drawPixmap(QRect(x, x , d->icon.width(), d->icon.height()) , d->icon);
    }
    drawText(painter, rect);
}
void Icon::paintDockView(QPainter * painter,const QRectF& rect)
{
    if (!d->icon.isNull()) {
        int x = (this->boundingRect().width() - d->icon.width())/2;
        painter->drawPixmap(QRect(x, x , d->icon.width(), d->icon.height()) , d->icon );
    }

    drawText(painter, rect);
}

void Icon::drawText(QPainter *painter, const QRectF& rect)
{
    float x = (this->boundingRect().width() - d->icon.width())/2;
    float y = boundingRect().height() - x;// - (d->icon.height() +x );
    QTextOption opt;
    opt.setAlignment(Qt::AlignHCenter);

    QFontMetrics fm(painter->font());
    QString label = fm.elidedText(d->text, Qt::ElideRight, boundingRect().width());
    
    QRectF tr = QRectF(0.0, y, boundingRect().width(), boundingRect().height() -
            (d->icon.height()));
    painter->drawText ( tr, label, opt);
}


}
