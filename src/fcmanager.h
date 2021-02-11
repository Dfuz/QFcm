#ifndef FCMANAGER_H
#define FCMANAGER_H

#include <QTcpServer>
#include "fcm_thread.h"

/**
 * TODO: 
 *  конфигурация ip, port, число подключений и тд...
 */

class FCManager : public QTcpServer
{
    Q_OBJECT
public:
    explicit FCManager(QObject *parent = 0);
    void readConfig(QString path);
    void startServer();

protected:
    void incomingConnection(qintptr socketDescriptor);

signals:

public slots:
};

#endif
