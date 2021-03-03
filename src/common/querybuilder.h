#ifndef QUERYBUILDER_H
#define QUERYBUILDER_H

#include <QTcpSocket>
#include <QFuture>

#include <algorithm>
#include <memory>
#include <type_traits>
#include <variant>
#include <functional>

#include "messagebuilder.h"

namespace Utils {

enum QueryDirection {
    Bidirectional,
    Unidirectional
};

template<MessageType ret>
using Verificator = std::function<bool(const Message<ret> &)>;

struct SharedSocket {
    SharedSocket(std::shared_ptr<QTcpSocket> _socket): socket(_socket) {}
protected:
    std::shared_ptr<QTcpSocket> socket;
};

template<QueryDirection direct, MessageType to, MessageType ret>
struct QueryFull: public SharedSocket {
    QueryFull(std::shared_ptr<QTcpSocket> _socket, const Message<to> _msg):
        SharedSocket(_socket), msg(_msg)
    {}

    template<QueryDirection T = direct,
             std::enable_if_t<T == QueryDirection::Bidirectional, bool> = true>
    std::optional<Message<ret>> invoke() noexcept {
        auto toSend = qCompress(msg.toJson(), compressonLevel);
        sendData(toSend);

        auto gotRaw = getData();
        auto got = Message<ret>::parseJson(qUncompress(gotRaw));

        if(!got.has_value())
            return std::nullopt;

        if (!std::all_of(verificators.cbegin(), verificators.cend(),
                        [&](auto fn) { return fn(got.value());}))
            return std::nullopt;

        return got;
    }

    template<QueryDirection T = direct,
             std::enable_if_t<T == QueryDirection::Unidirectional, bool> = true>
    std::optional<Message<MessageType::NoMessage>> invoke() noexcept {
        return {};
    }

    constexpr void setVerificator(Verificator<ret> verFn) noexcept {
        verificators.push_back(verFn);
    }
    constexpr void setCompression(int newLevel) noexcept {
        compressonLevel = newLevel;
    }

private:
    Message<to> msg;
    std::vector<Verificator<ret>> verificators;
    int compressonLevel = -1;

    //FIXME: implement
    void sendData(const QByteArray &data) {
        Q_UNUSED(data);
    }

    //FIXME: implement
    QByteArray getData() {
        return {};
    }
};

template<QueryDirection direct, MessageType ret>
struct QueryCanGet: public SharedSocket {
    template<MessageType to>
    QueryFull<direct, to, ret> toSend(const Message<to> & msg) noexcept {
        return {socket, msg};
    };
};

template<QueryDirection direct, MessageType to>
struct QueryCanSend: public SharedSocket {
    Message<to> msg;

    template<MessageType ret>
    QueryFull<direct, to, ret> toGet() noexcept {
        return {socket, msg};
    };
};

template<QueryDirection direct>
struct QueryBase: public SharedSocket {
    template<MessageType to>
    QueryCanSend<direct, to> toSend(const Message<to> &msg) noexcept {
        return {socket, msg};
    };

    template<MessageType ret>
    QueryCanGet<direct, ret> toGet() noexcept {
        return {socket};
    };
};

struct QueryBuilder {
public:
    QueryBuilder(QTcpSocket *skt): socket(skt) {}
    QueryBuilder(qintptr ptrskt) noexcept {
        QTcpSocket* skt = new QTcpSocket();
        skt->setSocketDescriptor(ptrskt);
        socket = std::make_shared<QTcpSocket>(skt);
    }

    template<QueryDirection direct = QueryDirection::Bidirectional>
    QueryBase<direct> makeQuery() noexcept {
        return {socket};
    };

    template<MessageType to>
    QueryFull<Unidirectional, to, NoMessage> onlySend(const Message<to> & msg) noexcept {
        return makeQuery<Unidirectional>()
                .toGet<NoMessage>()
                .toSend<to>(msg);
    }

    template<MessageType ret>
    QueryFull<Bidirectional, NoMessage, ret> onlyGet() noexcept {
        return makeQuery<Bidirectional>()
                .toSend<NoMessage>({})
                .toGet<ret>();
    }

private:
    std::shared_ptr<QTcpSocket> socket;
};

}

#endif // QUERYBUILDER_H
