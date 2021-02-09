#ifndef FCMTHREAD_H
#define FCMTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QDebug>

class FcmThread : public QThread
{
    Q_OBJECT
public:
    explicit FcmThread(qintptr ID, QObject *parent = 0);

    void run();

signals:
    void error(QTcpSocket::SocketError socketerror);

public slots:
    void readyRead();
    void disconnected();

private:
    QTcpSocket *socket;
    qintptr socketDescriptor;
};

#endif
