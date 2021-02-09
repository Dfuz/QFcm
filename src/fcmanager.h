#ifndef FCMANAGER_H
#define FCMANAGER_H

#include <QTcpServer>
#include "fcm_thread.h"

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

#endif
