#ifndef AGENTDATA_H
#define AGENTDATA_H

#include <QString>
#include <QVariant>

namespace FCM {

struct dataFromAgent
{
    QString hostName;   // видимое имя узла сети, которому принадлежит элемент данных
    QString keyData;    // тип данных
    QVariant value;
    quint16 virtualId;  // используется, чтобы отбрасывать дубликаты значений, которые могут быть отправлены в средах с плохой связью
    std::time_t clock;
};

}

#endif // AGENTDATA_H
