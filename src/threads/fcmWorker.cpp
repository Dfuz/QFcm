#include "fcmWorker.h"
#include "fcmanager.h"

FcmWorker::FcmWorker(quintptr id, QObject *parent) : QObject(parent), _id(id)
{}

void FcmWorker::processClient()
{
    qDebug() << "[Worker Thread]" << "[ID:" << QThread::currentThreadId() << "]" << "Поток запущен...";

    query = std::make_shared<Utils::QueryBuilder>(_id);

    //connect(query->socket.get(), &QTcpSocket::readyRead, this, &FcmWorker::readyRead, Qt::DirectConnection);
    connect(query->socket.get(), &QTcpSocket::disconnected, this, &FcmWorker::disconnected);
    
    auto agent = performHandshake(query);
    if (agent)
    {
        emit agentConnected(*agent);
        qDebug() << "[Worker Thread]" << "[ID:" << QThread::currentThreadId() << "]"
                 << "Клиент: " << getSocket()->peerAddress().toString() << ":"
                 << getSocket()->peerPort() << " подключился...";

        std::visit([&](auto&& arg)
        {
            using T = std::decay_t<decltype (arg)>;
            if constexpr (std::is_same_v<T, FCM::Agent>)
            {
                auto startTime = std::chrono::high_resolution_clock::now();

                auto response = query->onlyGet<Utils::Data>().invoke();
                if (!response)
                {
                    qDebug() << "Got no data";
                    query->socket->disconnect();
                    QThread::currentThread()->quit();
                }
                qDebug() << "Readed message: "  << response->jsonArrayData;
                int processedCount = 0, failedCount = 0;

                for (const auto& value: response->jsonArrayData)
                {
                    if (value.isObject())
                    {
                        auto const obj = value.toObject();
                        FCM::dataFromAgent data
                        {
                            .hostName = obj.value("hostname").toString("error"),
                            .keyData = obj.value("key").toString("error"),
                            .value = obj.value("value").toString("error"),
                            .virtualId = static_cast<quint16>(obj.value("id").toInt(0)),
                            .clock = obj.value("clock").toInt(-1),
                        };

                        if (data.checkData())
                        {
                            agentDataArray.push_back(data);
                            ++processedCount;
                        } else ++failedCount;
                    }
                }
                auto endTime = std::chrono::high_resolution_clock::now();
                auto durationTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
                QString info = QString("processed: %1; failed: %2; total: %3; microseconds spent: %4")
                               .arg(processedCount).arg(failedCount).arg(failedCount + processedCount).arg(durationTime);
                QString responseStatus = (failedCount > 0 or processedCount == 0) ? QString("failed") : QString("success");

                auto payload = QVariantMap{{"response", responseStatus}, {"info", info}};
                query->onlySend(Utils::ServiceMessage{payload}).invoke();

                if (FCManager::getDataBaseState())
                    if (!addToDataBaseAgent(arg))
                        qDebug() << "Problems with data base instert";
            }
        }, *agent);
    }
}

std::optional<FCM::AgentVariant> FcmWorker::performHandshake(std::shared_ptr<Utils::QueryBuilder> _query)
{
    QMap<QString, QVariant> payload
    {
        {"who", "server"},
        {"hostname", FCManager::getHostName()},
        {"compression", FCManager::getCompression()}
    };

    auto response = _query->makeQuery()
            .toSend(Utils::HandshakeMessage{payload})  //FIXME: как ты передашь информацию о компрессии уже заюзав компрессию
            .toGet<Utils::Handshake>()
            .invoke();

    if (response)
    {
        if (response->who == "agent")
            return FCM::Agent
            {.hostName = response->hostname,
             .macAddress = response->macaddress
            };
        else if (response->who == "agent_proxy")
            return FCM::Proxy{};
    }

    return std::nullopt;
}

std::shared_ptr<QTcpSocket> FcmWorker::getSocket()
{
    return query->socket;
}

bool FcmWorker::addToDataBaseAgent(const FCM::Agent& agent)
{
    QStringList querys;
    querys.push_back(DataBase::checkAgentExists.arg(agent.hostName));
    querys.push_back(DataBase::insertAgent
                     .arg(agent.macAddress, agent.hostName, QString(getSocket()->peerAddress().toString() + ":"
                                                          + QString::number(getSocket()->peerPort())))
                     .arg(1));

    for (auto const& it : agentDataArray)
    {
        //qDebug() << it.value.toJsonObject() << "\n" << QJsonDocument(it.value.toJsonObject()).toJson(QJsonDocument::Compact) << "\n";
        //QString jsonString = QJsonDocument(it.value.toJsonObject()).toJson(QJsonDocument::Compact).toStdString().c_str();
        querys.push_back(DataBase::insertAgentData.arg(it.hostName, it.keyData)
                         .arg(static_cast<int>(it.clock)).arg(it.value.toString()));
    }
    emit addAgentData(querys);
    return true;
}

void FcmWorker::readyRead()
{}

void FcmWorker::disconnected()
{
    qDebug() << "[Worker Thread]" << "[ID:" << QThread::currentThreadId() << "]"
             << "Клиент: " << getSocket()->peerAddress().toString() << ":" << getSocket()->peerPort() << " отключился...";
    emit finished();
}
