#include "fcmWorker.h"
#include "fcmanager.h"

FcmWorker::FcmWorker(QObject *parent) : QObject(parent)
{

}

void FcmWorker::assingSocket(quintptr id)
{
    query = std::make_shared<Utils::QueryBuilder>(id);
}

void FcmWorker::doSomeWork()
{
    qDebug() << "[Worker Thread]" << "[ID:" << QThread::currentThreadId() << "]" << "Поток запущен...";

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
                // TODO: parse data
            }
        }, *agent);
    }

    qDebug() << "[Worker Thread]" << "[ID:" << QThread::currentThreadId() << "]"
            << "Клиент: " << getSocket()->peerAddress().toString() << ":" << getSocket()->peerPort() << " подключился...";
}

std::optional<FCM::AgentVariant> FcmWorker::performHandshake(std::shared_ptr<Utils::QueryBuilder> _query)
{
    auto message = Utils::ServiceMessage
    {{
//            std::pair("type", "handshake"),
            std::pair("compression", FCManager::getCompression()),
    }};

    auto response = _query->makeQuery()
            .toSend(message)
            .toGet<Utils::Service>()
            .setVerificator([](const auto &msg) {
                Q_UNUSED(msg);
                return true;
            })
            .invoke();

    if (response)
    {
        if (response.value().payload["who"] == "agent")
            return FCM::Agent{};
        else if (response.value().payload["who"] == "agent_proxy")
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
