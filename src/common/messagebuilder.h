#ifndef MESSAGEBUILDER_H
#define MESSAGEBUILDER_H

#include <QVariant>
#include <QJsonObject>
#include <QJsonDocument>
#include <memory>

namespace Utils {

enum MessageType {
    Service,
    Data,
    Test,
    NoMessage
};

template<MessageType type>
struct Message {
    QVariantMap payload;

    QByteArray toJson() {
        payload.insert("type", static_cast<int>(type));
        return QJsonDocument{QJsonObject::fromVariantMap(payload)}.toJson();
    }
};

using TestMessage = Message<MessageType::Test>;
using DataMessage = Message<MessageType::Data>;
using ServiceMessage = Message<MessageType::Service>;

}

#endif // MESSAGEBUILDER_H
