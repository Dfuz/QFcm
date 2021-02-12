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

    _port = settings.value("port", 1234).toInt();
    if (settings.value("port").isNull()) settings.setValue("port", _port);

    _addr = QHostAddress{settings.value("address", "127.0.0.1").toString()};
    if (settings.value("address").isNull()) settings.setValue("address", _addr.toString());

    _max_number_of_agents = settings.value("max_agents", 4).toInt();
    if (settings.value("max_agents").isNull()) settings.setValue("max_agents", _max_number_of_agents);
}
