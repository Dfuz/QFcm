#include "fcmanager.h"
#include "fcm_thread.h"
#include "utils.h"

FCManager::FCManager(QObject *parent) :
    QTcpServer(parent)
{
}

void FCManager::startServer()
{
    readConfig();

    if(!this->listen(_addr, _port))
    {
        qDebug() << "Could not start server";
    }
    else
    {
        qDebug() << "Listening to port " << _port << "...";
    }
}

void FCManager::incomingConnection(qintptr socketDescriptor)
{
    if (_curr_number_of_agents >= _max_number_of_agents) {
        qInfo()<<"Too much agents";
        return;
    }

    qDebug() << socketDescriptor << " Connecting...";

    FcmThread *thread = new FcmThread(socketDescriptor, this);

    // once a thread is not needed, it will be beleted later
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
    
    _curr_number_of_agents++;
}

void FCManager::readConfig()
{
    QSettings settings(settings_path, Utils::JsonFormat);

    settings_path = settings.fileName();

    if (settings.value("port").isNull()) settings.setValue("port", 1234);
    _port = settings.value("port").toInt();

    if (settings.value("address").isNull()) settings.setValue("address", "127.0.0.1");
    _addr = QHostAddress{settings.value("address").toString()};

    if (settings.value("max_agents").isNull()) settings.setValue("max_agents", 4);
    _max_number_of_agents = settings.value("max_agents").toInt();
}
