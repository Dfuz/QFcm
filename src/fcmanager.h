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
#include <chrono>

//#include "fcm_thread.h"
#include "common/agentdata.h"
#include "agents/agentsinfo.h"

using namespace std::chrono;

/**
 * TODO: 
 *  конфигурация ip, port, число подключений и тд...
 */

struct FCConfig
{
    QHostAddress addr{QHostAddress::Null};
    std::chrono::milliseconds timeOut{0};
    int maxNumberOfAgents{0};
    int port{0};
};

class FCManager final : public QTcpServer
{
    Q_OBJECT

public:
    explicit FCManager(QObject *parent = 0);

    void readConfig(QString settings_path = "conf.json");
    bool startServer();
    bool setAgent(qint32, const AgentVariant &);
    friend class fcmanager_tests;

protected:
    void incomingConnection(qintptr socketDescriptor);

private:
    QHostAddress addr{QHostAddress::Null};
    std::chrono::milliseconds timeOut{0};
    int maxNumberOfAgents{0};
    int currNumberOfAgents{0};
    int port{0};

    //qint32 is QHostAddress::toIPv4Address()
    //QMap<qint32, AgentVariant> agents;
    QMutex agentsMutex;

signals:
    void gotData(const QPair<qint32, Common::dataFromAgent> &);

private slots:
};

#endif
