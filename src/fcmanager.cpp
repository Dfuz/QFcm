#include "fcmanager.h"
#include "threads/fcmWorker.h"
#include "common/utils.h"

FCManager::FCManager(QObject *parent) :
    QTcpServer(parent)
{
    qRegisterMetaType<FCM::AgentVariant>();
    dbusAdapter = std::make_unique<FcmAdapter>(this);
}

bool FCManager::startServer()
{
    if(!this->listen(ipAddress, port))
    {
        qInfo() << QTime::currentTime().toString(Qt::ISODateWithMs) << "Не удалось запустить сервер";
        qInfo() << "Адрес:" << ipAddress.toString().trimmed().remove('\"') << "порт:" << port << "...";
        qInfo() << "Ошибка:" << this->errorString();
        return false;
    }
    else
    {
        qInfo() << QTime::currentTime().toString(Qt::ISODateWithMs) << "Сервер запустился";
        qInfo() << "Адрес:" << ipAddress.toString().trimmed().remove('\"') << "порт:" << port << "...";
    }
    return true;
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
    connect(threadWorker, &FcmWorker::addAgentData, this, &FCManager::addToDataBaseAgent, Qt::ConnectionType::QueuedConnection);
    connect(this, &FCManager::newData, this->dbusAdapter.get(), &FcmAdapter::dataJsonList, Qt::ConnectionType::QueuedConnection);
    connect(this, &FCManager::newAgents, this->dbusAdapter.get(), &FcmAdapter::agentsJsonList, Qt::ConnectionType::QueuedConnection);

    threadWorker->moveToThread(thread);

    thread->start();
    qDebug() << "[Main Thread  ]" << "[ID:" << QThread::currentThreadId() << "]" << "Поток запущен...";
    return;
}

inline const QString FCManager::parseSqlQuery(const QString &query) const
{
    QVariantMap jsonMap;
    if (query.contains("INSERT", Qt::CaseInsensitive))
    {
        QRegularExpression regexp(R"(\((.*?)\))");
        qDebug() << regexp;
        auto match_it = regexp.globalMatch(query);

        if (!match_it.hasNext())
            return {};

        auto keyList = match_it.next().captured(0).remove(QRegExp("[( ')]")).split(QLatin1Char(','), QString::SkipEmptyParts);

        auto valueList = match_it.hasNext() ?
                match_it.next().captured(0).remove(QRegExp("[( ')]")).split(QLatin1Char(','), QString::SkipEmptyParts) : QStringList();

        if (keyList.size() != valueList.size())
            return {};
        for (int i = 0; i < keyList.size(); ++i)
            jsonMap.insert(keyList[i], valueList[i]);
    }
    return QJsonDocument(QJsonObject::fromVariantMap(jsonMap)).toJson(QJsonDocument::Compact);
}

void FCManager::readConfig(QString settings_path)
{
    QSettings settings(settings_path, Utils::JsonFormat);

    settings_path = settings.fileName();

    if (!settings.value("port").isNull())
        port = settings.value("port").toInt();

    if (!settings.value("address").isNull())
        ipAddress = QHostAddress{settings.value("address").toString()};

    if (!settings.value("max_agents").isNull())
        maxNumberOfAgents = settings.value("max_agents").toInt();

    if (!settings.value("compression").isNull())
        compression = settings.value("compression").toInt();

    // если имя хоста не задано, то в качестве имени будет взят хэш от мак-адреса
    if (settings.value("HostName").isNull())
        hostName =  QCryptographicHash::hash(Utils::getMacAddress().toUtf8(),
                                              QCryptographicHash::Md4).toBase64();
    else hostName = settings.value("HostName").toString();

    if (!settings.value("databasefile").isNull())
        dataBaseName = settings.value("databasefile").toString();
}

bool FCManager::initDBConnection()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("QFcm.db");
    if (!QFile::exists("QFcm.db"))
        qDebug() << "Не удалось найти базу данных, создаем новую...";
    dataBaseState = db.open();
    if (!dataBaseState)
    {
        qDebug() << "Не удалось открыть базу данных";
        return false;
    }
    else
    {
        foreach(QString queryStr, DataBase::createDataBase.split(";", QString::SkipEmptyParts))
            db.exec(queryStr);
        db.exec(DataBase::foreignKeysOn);
        db.commit();
        return true;
    }
}

void FCManager::addToDataBaseAgent(const QStringList& list)
{
    if (!db.isOpen())
        return;
    QSqlQuery query;
    QStringList jsonListData, jsonListAgent;
    qDebug() << "First query: " << list.at(0);
    qDebug() << "Second query: " << list.at(1);
    query.exec(list.at(0));
    if (!query.next())
    {
        qInfo() << "Adding new Agent...";
        if (query.exec(list.at(1)))
        {
            qInfo() << "Agent added successfully!";
            jsonListAgent << parseSqlQuery(list.at(1));
            qDebug() << jsonListAgent;
        }
        else qWarning() << db.lastError().text();
    }

    for (int it = 2; it < list.size(); ++it)
    {
        qDebug() << it << " value: " << list.at(it);
        if (!query.exec(list.at(it)))
            qDebug() << "Something went wrong... (SQL insertion)";
        else jsonListData << parseSqlQuery(list.at(it));
    }
    db.commit();
    qDebug() << jsonListData;
    emit newAgents(jsonListAgent);
    emit newData(jsonListData);
}

void FCManager::justExecQuery(const QString& query)
{
    if (!db.isOpen())
        return;

    QSqlQuery sqlQuery;
    if (!sqlQuery.exec(query))
        qDebug() << "SQL query ended with an error" << db.lastError().text();
}

bool FCManager::deleteAgent(const QString& hostName)
{
    if (!db.isOpen())
        return false;

    QSqlQuery query;
    query.prepare(DataBase::deleteAgent.arg(hostName));
    return query.exec();
}

// GETTERS
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

QStringList FCManager::getAllAgents()
{
    if (!db.isOpen())
        return {};
    QStringList list;
    QString string;
    auto query = db.exec(DataBase::selectAllAgents);
    while(query.next())
    {
        auto record = query.record();
        for (int i = 0; i < record.count(); ++i)
            string = record.value(i).toString();
        list << string;
    }
    return list;
}

QString FCManager::getAllAgentData(const QString &agent)
{
    if (!db.isOpen())
        return {};
    QString stringJSON;
    auto query = db.exec(DataBase::selectAllAgentDataJSON.arg(agent));
    while(query.next())
    {
        auto record = query.record();
        for (int i = 0; i < record.count(); ++i)
            stringJSON += record.value(i).toString();
    }
    return stringJSON;
}

int FCManager::getMaxNumberOfAgents() const
{
    return maxNumberOfAgents;
}

// SETTERS
void FCManager::setCompression(const int &newCompress)
{
    compression = newCompress;
}

void FCManager::setHostName(const QString &newHostName)
{
    hostName = newHostName;
}

void FCManager::setIpAddress(const QString &newIpAddress)
{
    ipAddress = QHostAddress{newIpAddress};
}

void FCManager::setDBFile(const QString &newDBName)
{
    dataBaseName = newDBName;
}

void FCManager::setPort(const int &newPort)
{
    port = newPort;
}

void FCManager::setMaxNumberOfAgents(int newMaxNumberOfAgents)
{
    maxNumberOfAgents = newMaxNumberOfAgents;
}
