#include "fcmanager.h"
#include "threads/fcm_thread.h"
#include "common/utils.h"

FCManager::FCManager(QObject *parent) :
    QTcpServer(parent)
{}

bool FCManager::startServer()
{
    readConfig();

    if(!this->listen(addr, port))
    {
        qDebug() << "Не удалось запустить сервер";
    }
    else
    {
        qDebug() << "Прослушивается порт " << port << "...";
//        pollingRate.start(config.timeOut);
    }

    return true;
}

//thread-safe
bool FCManager::setAgent(qint32 addr, const FCM::AgentVariant &info)
{
    QMutexLocker locker(&agentsMutex);
    if (maxNumberOfAgents >= currNumberOfAgents)
        return false;

    currNumberOfAgents++;

    agents.emplace(addr, info);
    return true;
}

void FCManager::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << socketDescriptor << " Подключение...";

//    FcmThread *thread = new FcmThread(socketDescriptor, this);

//    // когда поток завершится, его объект удалится
//    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

//    thread->start();



    // Предпологаю что мы делаем какой нить "handshake" и только потом
    // В потоке добавляем агента...
    // Но мб имеет смысл и держать поток на соединения, я хз...
    // И тада добалять в QMap agents потоки
}

void FCManager::readConfig(QString settings_path)
{
    QSettings settings(settings_path, Utils::JsonFormat);

    settings_path = settings.fileName();

    if (settings.value("port").isNull())
        settings.setValue("port", 1234);
    port = settings.value("port").toInt();

    if (settings.value("address").isNull())
        settings.setValue("address", "127.0.0.1");
    addr = QHostAddress{settings.value("address").toString()};

    if (settings.value("max_agents").isNull())
        settings.setValue("max_agents", 4);
    maxNumberOfAgents = settings.value("max_agents").toInt();

    if (settings.value("polling_rate").isNull())
        settings.setValue("polling_rate", "1m");
    timeOut = Utils::parseTime(settings.value("polling_rate").toString());
}
