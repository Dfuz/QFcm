#ifndef FCMTHREAD_H
#define FCMTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>
#include <type_traits>

#include "agents/agentsinfo.h"
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
    void doSomeWork();
    static std::optional<FCM::AgentVariant> performHandshake(std::shared_ptr<Utils::QueryBuilder>);
    std::shared_ptr<QTcpSocket> getSocket();
    std::shared_ptr<QString> hostName;
    std::shared_ptr<std::vector<FCM::dataFromAgent>> agentDataArray;
    friend class fcmanager_tests;

private:
    std::shared_ptr<Utils::QueryBuilder> query;
    quintptr _id;

signals:
    void resultReady(const QString& result);
    void error(QTcpSocket::SocketError socketerror);
    void agentConnected(FCM::AgentVariant);
    void finished();

public slots:
    void readyRead();
    void disconnected();
};

#endif
