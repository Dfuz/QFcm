#ifndef QUERYBUILDER_H
#define QUERYBUILDER_H

#include <QTcpSocket>
#include <QFuture>
#include <memory>
#include <type_traits>
#include <variant>

#include "messagebuilder.h"

namespace Utils {

enum QueryDirection {
    Bidirectional,
    Unidirectional
};

template<QueryDirection direct, MessageType type, MessageType ret>
struct QueryFull {
    std::unique_ptr<Message<type>> msg;

    //TODO: fucking enable_if.., SOMEONE HEEEELPP
    QFuture<void> invoke() {return QFuture<void>();};
};

template<QueryDirection direct, MessageType ret>
struct QueryCanGet {

    template<MessageType type>
    QueryFull<direct, type, ret> toSend(Message<type> msg) {
        return {std::make_unique<Message<type>>(msg)};
    };

//    QFuture<Message<ret>> invoke();
};

template<QueryDirection direct, MessageType type>
struct QueryCanSend {
    std::unique_ptr<Message<type>> msg;

    template<MessageType ret>
    QueryFull<direct, type, ret> toGet() {
        return {std::move(msg)};
    };

//    QFuture<void> invoke();
};

template<QueryDirection direct>
struct QueryBase {

    template<MessageType type>
    QueryCanSend<direct, type> toSend(Message<type> msg) {
        return {std::make_unique<Message<type>>(msg)};
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

private:
    std::unique_ptr<QTcpSocket> socket;
};

}

#endif // QUERYBUILDER_H
