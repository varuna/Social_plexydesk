#ifndef TWEET_SUPPLIER_H
#define TWEET_SUPPLIER_H

//#include "plexy.h"
#include <QMap>
#include <QStringList>
#include <QPainter>
#include <QList>
#include <QVariantMap>
class TweetSupplier
{
public :
        TweetSupplier();
	virtual ~TweetSupplier();
        QVariantMap getPublications(QString pluginMethod);
        bool getAuthentication(QString username,QString password);
};

#endif	

