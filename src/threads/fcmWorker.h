#ifndef FCMTHREAD_H
#define FCMTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QHostAddress>
#include <QtSql/QSqlDatabase>
#include <QtSql>
#include <type_traits>
#include <QDebug>
#include <Qt>

#include "agents/agentsinfo.h"
#include "common/database_query.h"
#include "agents/agentdata.h"
#include "common/querybuilder.h"
#include "common/messagebuilder.h"
#include "common/message_spec/messagetypes.h"
#include "common/message_spec/messagesendable.h"

class FCManager;

class FcmWorker : public QObject
{
    Q_OBJECT

public:
    FcmWorker(quintptr id, QObject *parent = 0);
    void processClient();
    static std::optional<FCM::AgentVariant> performHandshake(std::shared_ptr<Utils::QueryBuilder>);
    std::shared_ptr<QTcpSocket> getSocket();
    QString hostName;
    std::vector<FCM::dataFromAgent> agentDataArray;
    friend class fcmanager_tests;

private:
    std::shared_ptr<Utils::QueryBuilder> query;
    bool addToDataBaseAgent(const FCM::Agent& agent);
    bool dataBaseState{0};
    quintptr _id;

signals:
    void resultReady(const QString& result);
    void error(QTcpSocket::SocketError socketerror);
    void agentConnected(FCM::AgentVariant);
    void addAgentData(const QStringList& sqlQuerys);
    void finished();

public slots:
    void readyRead();
    void disconnected();
};

#endif
