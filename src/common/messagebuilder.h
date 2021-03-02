#ifndef MESSAGEBUILDER_H
#define MESSAGEBUILDER_H

#include <QVariant>
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
};

using TestMessage = Message<MessageType::Test>;
using DataMessage = Message<MessageType::Data>;
using ServiceMessage = Message<MessageType::Service>;

}

#endif // MESSAGEBUILDER_H
