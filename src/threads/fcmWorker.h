#ifndef FCMTHREAD_H
#define FCMTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QDebug>
#include <type_traits>
#include "fcmanager.h"
#include "agents/agentsinfo.h"
#include "common/querybuilder.h"
#include "common/messagebuilder.h"

class FCManager;

class FcmWorker : public QObject
{
    Q_OBJECT

public:
    FcmWorker(qintptr ID, QObject *parent = 0);
    void doSomeWork();

private:
    std::optional<FCM::AgentVariant> performHandshake();
    Utils::QueryBuilder query;

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
