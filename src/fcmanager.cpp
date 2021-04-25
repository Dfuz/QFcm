#include "fcmanager.h"
#include "threads/fcmWorker.h"
#include "common/utils.h"

FCManager::FCManager(QObject *parent) :
    QTcpServer(parent)
{
    qRegisterMetaType<FCM::AgentVariant>();
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("QFcm.sqlite");
    if (!QFile::exists("QFcm.sqlite"))
        qDebug() << "Не удалось найти базу данных, создаем новую...";
    dataBaseState = db.open();
    if (!dataBaseState)
        qDebug() << "Не удалось открыть базу данных";
    else
    {
        foreach(QString queryStr, DataBase::createDataBase.split(";", QString::SkipEmptyParts))
            db.exec(queryStr);
        db.exec(DataBase::foreignKeysOn);
        db.commit();
    }
}

bool FCManager::startServer()
{
    if(!this->listen(addr, port))
    {
        qDebug() << "Не удалось запустить сервер";
        qDebug() << "Адрес:" << addr.toString() << "порт:" << port << "...";
        qDebug() << "Ошибка:" << this->errorString();
        return false;
    }
    else
    {
        qDebug() << "Сервер запустился";
        qDebug() << "Адрес:" << addr.toString() << "порт:" << port << "...";
    }
    return true;
}

int FCManager::getCompression()
{
    return compression;
}

int FCManager::getDataBaseState()
{
    return dataBaseState;
}

QString FCManager::getHostName()
{
    return hostName;
}

void FCManager::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "[Main Thread  ]" << "[ID:" << QThread::currentThreadId() << "]"
             << socketDescriptor << " Подключение...";

    FcmWorker* threadWorker = new FcmWorker(socketDescriptor);

    qDebug() << "[Main Thread  ]" << "[ID:" << QThread::currentThreadId() << "]"
             << "Запускается новый поток для обрабоки клиента...";
    QThread* thread = new QThread();

    connect(thread, &QThread::started, threadWorker, &FcmWorker::processClient);
    connect(thread, &QThread::finished, threadWorker, &FcmWorker::deleteLater);
    connect(threadWorker, &FcmWorker::finished, thread, &QThread::quit);
    connect(threadWorker, &FcmWorker::finished, threadWorker, &FcmWorker::deleteLater);
    connect(threadWorker, &FcmWorker::addAgentData, this, &FCManager::addToDataBaseAgent);

    //threadWorker->db = QSqlDatabase::cloneDatabase(db, threadWorker->connectionName);
    //threadWorker->connectionName = "my_db_" + QUuid::createUuid().toString();

    /*connect(threadWorker, &FcmWorker::agentConnected, this, [&](const auto& agent){
        agents[threadWorker] = agent;
    }, Qt::QueuedConnection);

    connect(threadWorker, &FcmWorker::finished, this, [&](){
        agents.erase(threadWorker);
    }, Qt::QueuedConnection);*/

    //connect(threadWorker, &FcmWorker::agentConnected, this, &FCManager::agentConnectedRetranslate);

    threadWorker->moveToThread(thread);

    thread->start();
    qDebug() << "[Main Thread  ]" << "[ID:" << QThread::currentThreadId() << "]" << "Поток запущен...";
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

    if (settings.value("compression").isNull())
        settings.setValue("compression", 0);
    compression = settings.value("compression").toInt();

    // если имя хоста не задано, то в качестве имени будет взят хэш от мак-адреса
    if (settings.value("HostName").isNull())
        hostName =  QCryptographicHash::hash(Utils::getMacAddress().toUtf8(),
                                              QCryptographicHash::Md4).toBase64();
    else hostName = settings.value("HostName").toString();

    if (settings.value("polling_rate").isNull())
        settings.setValue("polling_rate", "1m");
    timeOut = Utils::parseTime(settings.value("polling_rate").toString());
}

void FCManager::addToDataBaseAgent(const QStringList& list)
{
    QSqlQuery query(list.at(0));
    qDebug() << "First query: " << list.at(0);
    qDebug() << "Second query: " << list.at(1);
    if (!query.next())
    {
        qDebug() << "Adding new Agent...";
        if (query.exec(list.at(1)))
            qDebug() << "Agent added successfully!";
        else qDebug() << db.lastError();
    }

    for (int it = 2; it < list.size(); ++it)
    {
        qDebug() << it << " value: " << list.at(it);
        if (!query.exec(list.at(it)))
            qDebug() << "Something went wrong... (SQL insertion)";
    }
    db.commit();
}
