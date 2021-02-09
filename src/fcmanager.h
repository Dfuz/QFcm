#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include "server_thread.h"

class FCManager : public QTcpServer
{
    Q_OBJECT
public:
    explicit FCManager(QObject *parent = 0);
    void startServer();
signals:

public slots:

protected:
    void incomingConnection(qintptr socketDescriptor);

};

#endif // MYSERVER_H
