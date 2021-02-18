#ifndef FCMANAGER_H
#define FCMANAGER_H

#include <QTcpServer>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QTimer>
#include <QRegularExpression>
#include <chrono>

#include "fcm_thread.h"

using namespace std::chrono;

/**
 * TODO: 
 *  конфигурация ip, port, число подключений и тд...
 */

class FCManager final : public QTcpServer
{
    Q_OBJECT
public:
    explicit FCManager(QObject *parent = 0);

    QString settings_path{"conf.json"};
    void readConfig();
    void startServer();
//TO_REMOVE
    int getPort() {return port;}
    QHostAddress getAddr() {return addr;}
    int get_max_number_of_agents() {return maxNumberOfAgents;}
    std::chrono::milliseconds getTimeOut() {return timeOut;}
//_________

protected:
    void incomingConnection(qintptr socketDescriptor);

private:
    QHostAddress addr{QHostAddress::Null};
    std::chrono::milliseconds timeOut{0};
    QTimer pollingRate{this};
    int maxNumberOfAgents{0};
    int currNumberOfAgents{0};
    int port{0};

signals:

private slots:
    void startPolling();
};

#endif
