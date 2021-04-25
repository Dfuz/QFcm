#ifndef UTILS_H
#define UTILS_H

#include <QSettings>
#include <QIODevice>
#include <QJsonObject>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QtNetwork>
#include <QDebug>

namespace Utils {

inline bool readJsonFile(QIODevice &device, QSettings::SettingsMap &map)
{
    QByteArray readedBytes = device.readAll();
    QJsonDocument resJson = QJsonDocument::fromJson(readedBytes);
    if (resJson.isNull())
    {
        qWarning() << "JSON: got null";
        return false;
    }
    if (!resJson.isObject())
    {
        qInfo() << resJson << " is not JSON object (wrong format)";
        return false;
    }
    map = resJson.object().toVariantMap();
    return true;
}

inline bool writeJsonFile(QIODevice &device, const QSettings::SettingsMap &map)
{
    auto resJsonObj = QJsonObject::fromVariantMap(map);
    QJsonDocument resJson(resJsonObj);
    auto written = device.write(resJson.toJson());
    if (written == -1) {
        return false;
    } else {
        qDebug() << "JSON: Written " << written << " bytes";
        return true;
    }
}

const QSettings::Format JsonFormat = QSettings::registerFormat("json", readJsonFile, writeJsonFile);

inline std::chrono::milliseconds parseTime(const QString& input)
{
    std::chrono::milliseconds time{0};

    QRegularExpression ms{"[0-9]+ms"},
    s{"[0-9]+s"},
    min{"[0-9]+m"},
    hour{"[0-9]+h"};

    auto const msMatch   = ms.match(input).captured(),
               sMatch    =  s.match(input).captured(),
               minMatch  = min.match(input).captured(),
               hourMatch = hour.match(input).captured();

    auto const pos_ms    = msMatch.lastIndexOf("ms"),
               pos_s     = sMatch.lastIndexOf('s'),
               pos_min   = minMatch.lastIndexOf('m'),
               pos_hour  = hourMatch.lastIndexOf('h');

    time += std::chrono::hours{hourMatch.left(pos_hour).toInt()};
    time += std::chrono::minutes{minMatch.left(pos_min).toInt()};
    time += std::chrono::seconds{sMatch.left(pos_s).toInt()};
    time += std::chrono::milliseconds{msMatch.left(pos_ms).toInt()};
    return time;
}

inline QString getMacAddress()
{
    foreach(QNetworkInterface netInterface, QNetworkInterface::allInterfaces())
    {
        if (!(netInterface.flags() & QNetworkInterface::IsLoopBack))
            return netInterface.hardwareAddress();
    }
    return QString();
}


}

#endif // UTILS_H
