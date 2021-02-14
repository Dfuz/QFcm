#ifndef FCMANAGER_H
#define FCMANAGER_H

#include <QTcpServer>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

#include "fcm_thread.h"

/**
 * TODO: 
 *  конфигурация ip, port, число подключений и тд...
 */

class FCManager : public QTcpServer
{
    Q_OBJECT
public:
    QString settings_path{"conf.json"};
    void readConfig();

    explicit FCManager(QObject *parent = 0);
    void startServer();
//TO_REMOVE
    int port() {return _port;}
    QHostAddress addr() {return _addr;}
    int max_number_of_agents() {return _max_number_of_agents;}
//_________

protected:
    void incomingConnection(qintptr socketDescriptor);

private:
    int _port;
    QHostAddress _addr;
    int _max_number_of_agents;

    int _curr_number_of_agents = 0;

signals:

public slots:
};

#endif
