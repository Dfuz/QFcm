#include "fcmanager.h"
#include "fcm_thread.h"

FCManager::FCManager(QObject *parent) :
    QTcpServer(parent)
{
}

void FCManager::startServer()
{
    int port = 1234; // magic number FIXME: make cfg file (maybe JSON)

    if(!this->listen(QHostAddress::LocalHost, port))
    {
        qDebug() << "Could not start server";
    }
    else
    {
        qDebug() << "Listening to port " << port << "...";
    }
}

void FCManager::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << socketDescriptor << " Connecting...";

    FcmThread *thread = new FcmThread(socketDescriptor, this);

    // once a thread is not needed, it will be beleted later
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}
