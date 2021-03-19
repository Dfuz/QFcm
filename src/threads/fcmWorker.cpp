#include "fcmWorker.h"

FcmWorker::FcmWorker(qintptr ID, QObject *parent) : QObject(parent)
{
    query.setSocketDescriptor(ID);
}

void FcmWorker::doSomeWork()
{
    qDebug() << "Поток запущен!";

    connect(query.socket.get(), &QTcpSocket::readyRead, this, &FcmWorker::readyRead, Qt::DirectConnection);
    connect(query.socket.get(), &QTcpSocket::disconnected, this, &FcmWorker::disconnected);

    qDebug() << query.socket->socketDescriptor() << " Client connected";
}

std::optional<FCM::AgentVariant> FcmWorker::performHandshake()
{
    auto message = Utils::ServiceMessage
    {{
//            std::pair("type", "handshake"),
            std::pair("compression", FCManager::getCompression())
    }};

    auto response = query.makeQuery()
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
        {
            FCM::Agent agent;
            emit agentConnected(agent);
            return agent;
        }
        else if (response.value().payload["who"] == "agent_proxy")
        {
            FCM::Proxy agent_proxy;
            emit agentConnected(agent_proxy);
            return agent_proxy;
        }
    }

    return std::nullopt;
}

void FcmWorker::readyRead()
{
    auto agent = performHandshake();
    if (agent)
    {
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
    qDebug() << query.socket->socketDescriptor() << " Disconnected";
    emit finished();
}


