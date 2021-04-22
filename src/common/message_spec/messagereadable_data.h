#ifndef MESSAGREADABLE_DATA_H
#define MESSAGREADABLE_DATA_H

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <optional>
#include <memory>
#include <ctime>

#include "messagereadable.h"

namespace Utils
{

template<>
struct ReadableMessage<Utils::Data>
{
    std::shared_ptr<QJsonArray> jsonArrayData;
    std::time_t clock;

    static std::optional<ReadableMessage<Utils::Data>> parseJson(const QByteArray &data) noexcept
    {
        const auto obj = ReadableMessageHelper::verifyType<Utils::Data>(data);
        if (!obj)
            return std::nullopt;
        
        ReadableMessage<Utils::Data> retval{};
        retval.jsonArrayData = std::make_shared<QJsonArray>(obj->value("data").toArray());
        retval.clock = obj->value("clock").toInt();

        return retval;
    }    
};

}

#endif // MESSAGREADABLE_DATA_H
