#include "fcmanager.h"
#include "fcm_thread.h"
#include "utils.h"

FCManager::FCManager(QCoreApplication *app, QObject *parent) :
    QTcpServer(parent),
    eventLoop(app)
{
    QObject::connect(&pollingRate, &QTimer::timeout, this, &FCManager::startPolling);
}

bool FCManager::startServer()
{
    readConfig();
    if (eventLoop == nullptr) return false;

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

void FCManager::incomingConnection(qintptr socketDescriptor)
{
    if (currNumberOfAgents >= config.maxNumberOfAgents) {
        qInfo() << "Слишком много агентов";
        return;
    }

    qDebug() << socketDescriptor << " Подключение...";

    FcmThread *thread = new FcmThread(socketDescriptor, this);

    // когда поток завершится, его объект удалится
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
    
    currNumberOfAgents++;
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

void FCManager::readConfig()
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
    return;
}
