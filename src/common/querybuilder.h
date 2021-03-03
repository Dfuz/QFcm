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

template<QueryDirection direct, MessageType to, MessageType ret>
struct QueryFull {
    QueryFull(const Message<to> &_msg): msg(_msg) {}

    template<QueryDirection T = direct,
             std::enable_if_t<T == QueryDirection::Bidirectional, bool> = true>
    std::optional<Message<ret>> invoke() {
        Message<ret> got = {}; //TODO: actual code

        if (!std::all_of(verificators.cbegin(), verificators.cend(),
                        [&](const auto &fn) { return fn(got);}))
            return std::nullopt;

        return {};
    };

    template<QueryDirection T = direct,
             std::enable_if_t<T == QueryDirection::Unidirectional, bool> = true>
    Message<MessageType::NoMessage> invoke() {
        return {};
    };

    void setVerificator(Verificator<ret> verFn) {
        verificators.push_back(verFn);
    }

private:
    Message<to> msg;
    std::vector<Verificator<ret>> verificators;
};

template<QueryDirection direct, MessageType ret>
struct QueryCanGet {
    template<MessageType to>
    QueryFull<direct, to, ret> toSend(const Message<to> & msg) {
        return {msg};
    };
};

template<QueryDirection direct, MessageType to>
struct QueryCanSend {
    Message<to> msg;

    template<MessageType ret>
    QueryFull<direct, to, ret> toGet() {
        return {msg};
    };
};

template<QueryDirection direct>
struct QueryBase {
    template<MessageType to>
    QueryCanSend<direct, to> toSend(const Message<to> &msg) {
        return {msg};
    };

    template<MessageType ret>
    QueryCanGet<direct, ret> toGet() {
        return {};
    };
};

struct QueryBuilder {
public:
    QueryBuilder(QTcpSocket *skt): socket(skt) {}
    QueryBuilder(qintptr ptrskt) {
        QTcpSocket* skt = new QTcpSocket();
        skt->setSocketDescriptor(ptrskt);
        socket = std::make_unique<QTcpSocket>(skt);
    }

    template<QueryDirection direct = QueryDirection::Bidirectional>
    QueryBase<direct> makeQuery() {
        return {};
    };

    template<MessageType to>
    QueryFull<Unidirectional, to, NoMessage> onlySend(const Message<to> & msg) {
        return makeQuery<Unidirectional>()
                .toGet<NoMessage>()
                .toSend<to>(msg);
    }

    template<MessageType ret>
    QueryFull<Bidirectional, NoMessage, ret> onlyGet() {
        return makeQuery<Bidirectional>()
                .toSend<NoMessage>({})
                .toGet<ret>();
    }

private:
    std::unique_ptr<QTcpSocket> socket;
};

}

#endif // QUERYBUILDER_H
