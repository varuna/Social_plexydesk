#include <QObject>
#include <abstractplugininterface.h>


class TwitterPluginInterface : public PlexyDesk::AbstractPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PlexyDesk::AbstractPluginInterface);

    public :
        TwitterPluginInterface(QObject *parent=0);
        PlexyDesk::BasePlugin* instance();
};
