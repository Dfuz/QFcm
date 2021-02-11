#include "fcmanager.h"
#include "fcm_thread.h"

FCManager::FCManager(QObject *parent) :
    QTcpServer(parent)
{
}

void FCManager::startServer()
{
    int port = 1234; // magic number FIXME: make cfg file (maybe JSON)

    if(!this->listen(QHostAddress::LocalHost, port))
    {
        qDebug() << "Could not start server";
    }
    else
    {
        qDebug() << "Listening to port " << port << "...";
    }
}

void FCManager::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << socketDescriptor << " Connecting...";

    FcmThread *thread = new FcmThread(socketDescriptor, this);

    // once a thread is not needed, it will be beleted later
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}

inline bool readJsonFile(QIODevice &device, QSettings::SettingsMap &map)
{
    QByteArray readedBytes = device.readAll();
    QJsonDocument resJson(QJsonDocument::fromJson(readedBytes));
    if (resJson == null) {
        //TODO: handle error
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
    device.write(resJson.toJson());
}

void FCManager::readConfig(QString path)
{
    const QSettings::Format JsonFormat = QSettings::registerFormat("json", readJsonFile, writeJsonFile);
    QSettings settings(path, JsonFormat);

    
}
