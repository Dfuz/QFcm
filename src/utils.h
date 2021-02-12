#ifndef UTILS_H
#define UTILS_H

#include <QSettings>
#include <QIODevice>
#include <QJsonObject>
#include <QJsonDocument>

namespace Utils {

inline bool readJsonFile(QIODevice &device, QSettings::SettingsMap &map)
{
    QByteArray readedBytes = device.readAll();
    QJsonDocument resJson = QJsonDocument::fromJson(readedBytes);
    if (resJson.isNull()) {
        //TODO: handle error
        qWarning()<<"got null";
        return false;
    }
    if (!resJson.isObject()) {
        qInfo() << resJson << " is not Object (wrong format)";
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
        qInfo()<<"written "<<written<<" bytes";
        return true;
    }
}

const QSettings::Format JsonFormat = QSettings::registerFormat("json", readJsonFile, writeJsonFile);

}

#endif // UTILS_H
