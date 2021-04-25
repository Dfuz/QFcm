#include "fcmanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication application(argc, argv);
    FCManager server(&application);
    server.readConfig();
    server.initDBConnection();
    if (!server.startServer())
        application.exit(-1);
    return application.exec();
}
