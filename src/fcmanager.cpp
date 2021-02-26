#include "fcmanager.h"
#include "fcm_thread.h"
#include "utils.h"

FCManager::FCManager(QObject *parent) :
    QTcpServer(parent)
{
    QObject::connect(&pollingRate, &QTimer::timeout, this, &FCManager::startPolling);
}

bool FCManager::startServer()
{
    readConfig();

    if(!this->listen(config.addr, config.port))
    {
        qDebug() << "Не удалось запустить сервер";
    }
    else
    {
        qDebug() << "Прослушивается порт " << config.port << "...";
        pollingRate.start(config.timeOut);
    }

    return true;
}

//thread-safe
bool FCManager::setAgent(qint32 addr, const agentInfo &info)
{
    QMutexLocker locker(&agentsMutex);
    if (config.maxNumberOfAgents >= currNumberOfAgents)
        return false;

    currNumberOfAgents++;

    agents.insert(addr, info);
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
    FcmThreadHandshake{}.start();
}

//thread-safe
void FCManager::pollingFn()
{
    QMutexLocker locker(&agentsMutex);

    for(const auto agent: agents) {
        Q_UNUSED(agent);
        FcmThreadPoller{}.start();
        // если дата то emit FCManager::gotData(data);
    }
}

inline std::chrono::milliseconds parseTime(const QString& input)
{
    std::chrono::milliseconds time{0};

    QRegularExpression ms{"[0-9]+ms"},
                       s{"[0-9]+s"},
                       min{"[0-9]+m"},
                       hour{"[0-9]+h"};

    time += hours{hour.match(input).captured().left(1).toInt()};
    time += minutes{min.match(input).captured().left(1).toInt()};
    time += seconds{s.match(input).captured().left(1).toInt()};
    time += milliseconds{ms.match(input).captured().left(2).toInt()};
    return time;
}

void FCManager::readConfig(QString settings_path)
{
    QSettings settings(settings_path, Utils::JsonFormat);

    settings_path = settings.fileName();

    if (settings.value("port").isNull())
        settings.setValue("port", 1234);
    config.port = settings.value("port").toInt();

    if (settings.value("address").isNull())
        settings.setValue("address", "127.0.0.1");
    config.addr = QHostAddress{settings.value("address").toString()};

    if (settings.value("max_agents").isNull())
        settings.setValue("max_agents", 4);
    config.maxNumberOfAgents = settings.value("max_agents").toInt();

    if (settings.value("polling_rate").isNull())
        settings.setValue("polling_rate", "1m");
    config.timeOut = parseTime(settings.value("polling_rate").toString());
}


void FCManager::startPolling()
{
    pollingRate.callOnTimeout(this, &FCManager::pollingFn);
    pollingRate.start(config.timeOut);
}
