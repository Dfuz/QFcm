#include <QCoreApplication>
#include "fcmanager.h"

//FIXME: make cmd for manager
//FIXME: add tests
//FIXME: add JSON for communication between modules
//          and compression

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    FCManager server;
    server.startServer();

    return a.exec();
}
