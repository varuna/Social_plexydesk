
#ifndef PLEXY_WIDGET_PLUGIN_H
#define PLEXY_WIDGET_PLUGIN_H

#include <plexy.h>
#include <baseplugin.h>

class QGraphicsItem;

namespace PlexyDesk
{
    class VISIBLE_SYM WidgetPlugin : public BasePlugin
    {
        Q_OBJECT
    public :
        virtual ~WidgetPlugin() {}
        virtual QGraphicsItem * item() = 0;
    };
}
#endif
