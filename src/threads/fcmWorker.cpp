#include "fcmWorker.h"
#include "fcmanager.h"

FcmWorker::FcmWorker(qintptr ID, QObject *parent) : QObject(parent)
{
    query.setSocketDescriptor(ID);
    peerAddress = query.socket->peerAddress();
    peerPort = query.socket->peerPort();
}

void FcmWorker::doSomeWork()
{
    qDebug() << "[Worker Thread]" << "[ID:" << QThread::currentThreadId() << "]" << "Поток запущен...";

    connect(query.socket.get(), &QTcpSocket::readyRead, this, &FcmWorker::readyRead, Qt::DirectConnection);
    connect(query.socket.get(), &QTcpSocket::disconnected, this, &FcmWorker::disconnected);
    qDebug() << "[Worker Thread]" << "[ID:" << QThread::currentThreadId() << "]"
             << "Клиент: " << peerAddress.toString() << ":" << peerPort << " подключился...";
}

std::optional<FCM::AgentVariant> FcmWorker::performHandshake(Utils::QueryBuilder &_query)
{
    auto message = Utils::ServiceMessage
    {{
//            std::pair("type", "handshake"),
            std::pair("compression", FCManager::getCompression()),
    }};

    auto response = _query.makeQuery()
            .toSend(message)
            .toGet<Utils::Service>()
            .setVerificator([](const auto& arg){
        Q_UNUSED(arg);
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

void FcmWorker::readyRead()
{
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
                auto response = query.onlyGet<Utils::Data>().invoke();
                // TODO: parse data
            }
        }, *agent);
    }
//    QByteArray Data = socket->readAll();
//    qDebug() << socket->socketDescriptor() << " Data in: " << Data;
//    socket->write(Data);
}

void FcmWorker::disconnected()
{
    qDebug() << "[Worker Thread]" << "[ID:" << QThread::currentThreadId() << "]"
             << "Клиент: " << peerAddress.toString() << ":" << peerPort << " отключился...";
    emit finished();
}
