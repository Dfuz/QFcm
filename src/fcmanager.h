#ifndef FCMANAGER_H
#define FCMANAGER_H

#include <QCoreApplication>
#include <QTcpServer>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QTimer>
#include <QMutex>
#include <QMutexLocker>
#include <QRegularExpression>
#include <QtConcurrent/QtConcurrent>
#include <chrono>
#include <map>
#include <QMap>
#include "threads/fcmWorker.h"
#include "common/agentdata.h"
#include "agents/agentsinfo.h"

using namespace std::chrono;
class FcmWorker;

/**
 * TODO: 
 *  конфигурация ip, port, число подключений и тд...
 */
class FCManager final : public QTcpServer
{
    Q_OBJECT

public:
    explicit FCManager(QObject *parent = 0);
    void readConfig(QString settings_path = "conf.json");
    bool startServer();
    static int getCompression(void);
    friend class fcmanager_tests;

protected:
    void incomingConnection(qintptr socketDescriptor);

private:
    QHostAddress addr{QHostAddress::Null};
    std::chrono::milliseconds timeOut{0};
    int currNumberOfAgents{0};
    int maxNumberOfAgents{0};
    inline static int compression;
    int port{0};

    //qint32 is QHostAddress::toIPv4Address()
    std::map<FcmWorker*, FCM::AgentVariant> agents;
    std::map<QString, FCM::AgentVariant> allAgents;

    QMutex agentsMutex;

signals:
    void gotData(const QPair<qint32, Common::dataFromAgent> &);
    void agentConnectedRetranslate(FCM::AgentVariant);

private slots:
};

#endif
