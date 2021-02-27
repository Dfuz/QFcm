#include "fcmanager.h"
#include "threads/fcmWorker.h"
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
    }

    return true;
}

void FCManager::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << socketDescriptor << " Подключение...";
    FcmWorker* threadWorker = new FcmWorker(socketDescriptor, this);
    QThread* thread = new QThread();
    threadWorker->moveToThread(thread);

    connect(thread, &QThread::started, threadWorker, &FcmWorker::doSomeWork);
    connect(thread, &QThread::finished, threadWorker, &FcmWorker::deleteLater);
    connect(threadWorker, &FcmWorker::finished, thread, &QThread::quit);
    connect(threadWorker, &FcmWorker::finished, threadWorker, &FcmWorker::deleteLater);

    connect(threadWorker, &FcmWorker::agentConnected, this, [=](const auto& agent){
        agents[threadWorker] = agent;
    }, Qt::QueuedConnection);

    connect(threadWorker, &FcmWorker::finished, this, [=](){
        agents.erase(threadWorker);
    }, Qt::QueuedConnection);

    thread->start();
    return;
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
