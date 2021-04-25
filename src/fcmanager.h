#ifndef FCMANAGER_H
#define FCMANAGER_H

#include <QtConcurrent/QtConcurrent>
#include <QRegularExpression>
#include <QtSql/QSqlDatabase>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QMutexLocker>
#include <QJsonObject>
#include <QTcpServer>
#include <QSettings>
#include <QTimer>
#include <QMutex>
#include <QFile>
#include <QtSql>
#include <QMap>

#include <chrono>
#include <map>

#include "threads/fcmWorker.h"
#include "agents/agentsinfo.h"
#include "agents/agentdata.h"
#include "common/message_spec/messagesendable.h"
#include "common/message_spec/messagetypes.h"
#include "common/messagebuilder.h"
#include "common/database_query.h"
#include "common/utils.h"

using namespace std::chrono;
class FcmWorker;

/**
 * TODO: 
 *  конфигурация ip, port, число подключений и тд...
 */
class FCManager final : public QTcpServer
{
    Q_OBJECT

public:
    explicit FCManager(QObject *parent = 0);
    void readConfig(QString settings_path = "conf.json");
    bool initDBConnection();
    bool startServer();
    static int getCompression(void);
    static int getDataBaseState(void);
    static QString getHostName(void);
    friend class fcmanager_tests;

protected:
    void incomingConnection(qintptr socketDescriptor);

private:
    QHostAddress addr{QHostAddress::Null};
    std::chrono::milliseconds timeOut{0};
    int currNumberOfAgents{0};
    int maxNumberOfAgents{0};
    inline static QString dataBaseName{"QFcm.db"};
    inline static bool dataBaseState{false};
    inline static QString hostName;
    inline static int compression;
    int port{0};
    QSqlDatabase db;

signals:
    void gotData(const QPair<qint32, FCM::dataFromAgent> &);
    void agentConnectedRetranslate(FCM::AgentVariant);

private slots:
    void addToDataBaseAgent(const QStringList& list);
    void justExecQuery(const QString& query);
};

#endif
