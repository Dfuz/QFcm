#include <QCoreApplication>
#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    FCManager server;
    server.startServer();

    return a.exec();
}
