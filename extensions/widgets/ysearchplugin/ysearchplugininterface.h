#include <QObject>
#include <abstractplugininterface.h>


class YSearchPluginInterface : public PlexyDesk::AbstractPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PlexyDesk::AbstractPluginInterface);

    public :
        YSearchPluginInterface(QObject *parent=0);
        PlexyDesk::BasePlugin* instance();
};
