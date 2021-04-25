#ifndef AGENTDATA_H
#define AGENTDATA_H

#include <QString>
#include <QJsonValue>
#include <QVariant>
#include <QJsonDocument>

namespace FCM {

struct dataFromAgent
{
    QString hostName;   // видимое имя узла сети, которому принадлежит элемент данных
    QString keyData;    // тип данных
    QVariant value;
    quint16 virtualId;  // используется, чтобы отбрасывать дубликаты значений, которые могут быть отправлены в средах с плохой связью
    std::time_t clock;

    bool checkData() const
    {
        if (hostName == "error" or keyData == "error" or value.isNull())
            return false;
        if (virtualId == 0 or clock == -1)
            return false;
        return true;
    }
};

}

#endif // AGENTDATA_H
