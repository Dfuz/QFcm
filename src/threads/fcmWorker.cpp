#include "fcmWorker.h"
#include "fcmanager.h"

FcmWorker::FcmWorker(quintptr id, QObject *parent) : QObject(parent), _id(id)
{}

void FcmWorker::doSomeWork()
{
    qDebug() << "[Worker Thread]" << "[ID:" << QThread::currentThreadId() << "]" << "Поток запущен...";

    query = std::make_shared<Utils::QueryBuilder>(_id);

    connect(query->socket.get(), &QTcpSocket::readyRead, this, &FcmWorker::readyRead, Qt::DirectConnection);
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
                qDebug() << "Readed message: "  << *response->jsonArrayData;
                agentDataArray = std::make_shared<std::vector<FCM::dataFromAgent>>();
                int processedCount = 0, failedCount = 0;

                foreach (const auto& value, *response->jsonArrayData)
                {
                    if (value.isObject())
                    {
                        FCM::dataFromAgent data;
                        auto const obj = value.toObject();
                        data.hostName   = obj.value("hostname").toString("error");
                        data.keyData    = obj.value("key").toString("error");
                        data.virtualId  = obj.value("id").toInt(0);
                        data.clock      = obj.value("clock").toInt(-1);
                        data.value      = obj.value("value").toString("error");

                        if (data.checkData())
                        {
                            agentDataArray->push_back(data);
                            ++processedCount;
                        } else ++failedCount;
                    }
                }
                auto endTime = std::chrono::high_resolution_clock::now();
                auto durationTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
                QString info = QString("processed: %1; failed: %2; total: %3; seconds spent: %4")
                               .arg(processedCount).arg(failedCount).arg(failedCount + processedCount).arg(durationTime);
                QString responseStatus = (failedCount > 0 or processedCount == 0) ? QString("failed") : QString("success");

                auto payload = QVariantMap{{"response", responseStatus}, {"info", info}};
                query->onlySend(Utils::ServiceMessage{payload}).invoke();
            }
        }, *agent);
    }
}

std::optional<FCM::AgentVariant> FcmWorker::performHandshake(std::shared_ptr<Utils::QueryBuilder> _query)
{
    QMap<QString, QVariant> payload = 
    {
        {"who", "server"},
        {"hostname", FCManager::getHostName()},
        {"compression", FCManager::getCompression()}
    };

    auto response = _query->makeQuery()
            .toSend(Utils::HandshakeMessage{payload})
            .toGet<Utils::Handshake>()
            .invoke();

    if (response)
    {
        if (response->who == "agent")
            return FCM::Agent{};
        else if (response->who == "agent_proxy")
            return FCM::Proxy{};
    }

    return std::nullopt;
}

std::shared_ptr<QTcpSocket> FcmWorker::getSocket()
{
    return query->socket;
}

void FcmWorker::readyRead()
{
//    QByteArray Data = socket->readAll();
//    qDebug() << socket->socketDescriptor() << " Data in: " << Data;
//    socket->write(Data);
}

void FcmWorker::disconnected()
{
    qDebug() << "[Worker Thread]" << "[ID:" << QThread::currentThreadId() << "]"
             << "Клиент: " << getSocket()->peerAddress().toString() << ":" << getSocket()->peerPort() << " отключился...";
    emit finished();
}
