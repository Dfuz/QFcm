#ifndef FCMANAGER_H
#define FCMANAGER_H

#include <QCoreApplication>
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

struct FCConfig
{
    QHostAddress addr{QHostAddress::Null};
    std::chrono::milliseconds timeOut{0};
    int maxNumberOfAgents{0};
    int port{0};
};

class FCManager final : public QTcpServer
{
    Q_OBJECT
public:
    explicit FCManager(QCoreApplication *, QObject *parent = 0);

    QString settings_path{"conf.json"};
    void readConfig();
    bool startServer();

//TESTS
    FCConfig getConfig() {return config;}
//_________

protected:
    void incomingConnection(qintptr socketDescriptor);

private:
    QCoreApplication *eventLoop;
    FCConfig config;
    QTimer pollingRate{this};
    int currNumberOfAgents{0};

signals:

private slots:
    void startPolling();
};

#endif
