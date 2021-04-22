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
        // AHTUNG!
        std::visit([&](auto&& arg)
        {
            using T = std::decay_t<decltype (arg)>;
            if constexpr (std::is_same_v<T, FCM::Agent>)
            {
                auto response = query->onlyGet<Utils::Data>().invoke();
                qDebug() << "Readed some shit: " << *response->jsonArrayData;

                // TODO: parse data
            }
        }, *agent);
    }

    qDebug() << "[Worker Thread]" << "[ID:" << QThread::currentThreadId() << "]"
            << "Клиент: " << getSocket()->peerAddress().toString() << ":" << getSocket()->peerPort() << " подключился...";
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
