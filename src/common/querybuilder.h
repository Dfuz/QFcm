#ifndef QUERYBUILDER_H
#define QUERYBUILDER_H

#include <QTcpSocket>
#include <QtConcurrent/QtConcurrent>

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

template<QueryDirection direct, MessageType to = NoMessage, MessageType ret = NoMessage>
struct Query {

    template<MessageType invRet>
    using InvokeReturn = std::optional<Message<invRet>>;

    Query(std::shared_ptr<QTcpSocket> _socket):
        socket(_socket)
    {}
    Query(std::shared_ptr<QTcpSocket> _socket, const Message<to> _msg):
        socket(_socket), msg(_msg)
    {}

    template<MessageType newTo>
    constexpr Query<direct, newTo, ret> toSend(const Message<newTo> & _msg) noexcept {
        return {socket, _msg};
    };

    template<MessageType newRet>
    constexpr Query<direct, to, newRet> toGet() noexcept {
        return {socket, msg};
    };

    template<QueryDirection T = direct,
             std::enable_if_t<T == Bidirectional, bool> = true>
    constexpr QFuture<InvokeReturn<ret>> invoke() noexcept {

        return QtConcurrent::run([&] () -> InvokeReturn<ret> {
            if constexpr (to != NoMessage) {
                auto toSend = qCompress(msg.toJson(), compressonLevel);
                socket->write(toSend);
            }

            if constexpr (ret == NoMessage)
                return Message<NoMessage>{};

            if (!socket->waitForReadyRead())
                return std::nullopt;

            auto gotRaw = socket->readAll();
            auto got = Message<ret>::parseJson(qUncompress(gotRaw));

            if(!got.has_value())
                return std::nullopt;

            if (!std::all_of(verificators.cbegin(), verificators.cend(),
                            [&](auto fn) { return fn(got.value());}))
                return std::nullopt;

            return got;
        });
    }

    template<QueryDirection T = direct,
             std::enable_if_t<T == Unidirectional, bool> = true>
    constexpr Message<NoMessage> invoke() noexcept {

        if constexpr (to != NoMessage) {
            auto toSend = qCompress(msg.toJson(), compressonLevel);
            socket->write(toSend);
        }

        return {};
    }

    constexpr Query& setVerificator(Verificator<ret> verFn) noexcept {
        verificators.push_back(verFn);
        return *this;
    }
    constexpr Query& setCompression(int newLevel) noexcept {
        compressonLevel = newLevel;
        return *this;
    }

    friend class fcmanager_tests;

private:
    std::shared_ptr<QTcpSocket> socket;
    Message<to> msg;
    std::vector<Verificator<ret>> verificators;
    int compressonLevel = -1;
};

struct QueryBuilder {
public:
    QueryBuilder(QTcpSocket *skt): socket(skt) {}
    QueryBuilder(qintptr ptrskt) noexcept {
        QTcpSocket* skt = new QTcpSocket();
        skt->setSocketDescriptor(ptrskt);
        socket = std::make_shared<QTcpSocket>(skt);
    }

    template<QueryDirection direct = Bidirectional>
    Query<direct> makeQuery() noexcept {
        return {socket};
    };

    template<MessageType to>
    Query<Unidirectional, to, NoMessage> onlySend(const Message<to> & msg) noexcept {
        return makeQuery<Unidirectional>()
                .toGet<NoMessage>()
                .toSend<to>(msg);
    }

    template<MessageType ret>
    Query<Bidirectional, NoMessage, ret> onlyGet() noexcept {
        return makeQuery<Bidirectional>()
                .toSend<NoMessage>({})
                .toGet<ret>();
    }

private:
    std::shared_ptr<QTcpSocket> socket;
};

}

#endif // QUERYBUILDER_H
