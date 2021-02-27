#ifndef FCMTHREAD_H
#define FCMTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QDebug>
#include "agents/agentsinfo.h"

class FcmWorker : public QObject
{
    Q_OBJECT

public:
    explicit FcmWorker(qintptr ID, QObject *parent = 0);
    void doSomeWork();

private:
    QTcpSocket *socket;
    std::optional<FCM::AgentVariant> performHandshake();

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
