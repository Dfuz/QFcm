#ifndef FCMANAGER_H
#define FCMANAGER_H

#include <QtConcurrent/QtConcurrent>
#include <QAtomicInteger>
#include <QRegularExpression>
#include <QtSql/QSqlDatabase>
#include <QCoreApplication>
#include <QDBusAbstractAdaptor>
#include <QDBusVariant>
#include <QDBusArgument>
#include <QDBusContext>
#include <QJsonDocument>
#include <QMutexLocker>
#include <QJsonObject>
#include <QTcpServer>
#include <QSettings>
#include <QTimer>
#include <QFile>
#include <QtSql>
#include <QMap>

#include <chrono>
#include <map>

#include "threads/fcmWorker.h"
#include "fcmanager_adapter.h"
#include "agents/agentsinfo.h"
#include "agents/agentdata.h"
#include "common/message_spec/messagesendable.h"
#include "common/message_spec/messagetypes.h"
#include "common/messagebuilder.h"
#include "common/database_query.h"
#include "common/utils.h"

using namespace std::chrono;
class FcmWorker;
class FcmAdapter;

class FCManager final : public QTcpServer
{
    Q_OBJECT

public:
    explicit FCManager(QObject *parent = 0);
    void readConfig(QString settings_path = "conf.json");
    bool initDBConnection();
    bool startServer();
    friend class fcmanager_tests;
    friend class FcmAdapter;

    // сеттеры
    void setIpAddress(const QString &newIpAddress);
    void setCompression(const int& newCompress);
    void setHostName(const QString& newHostName);
    void setDBFile(const QString& newDBName);
    void setPort(const int& newPort);
    void setMaxNumberOfAgents(int newMaxNumberOfAgents);

    // геттеры
    static int getCompression(void);
    static int getDataBaseState(void);
    static QString getHostName(void);
    QStringList getAllAgents(void);
    QString getAllAgentData(const QString& agent);
    int getMaxNumberOfAgents() const;

protected:
    void incomingConnection(qintptr socketDescriptor);

private:
    QHostAddress ipAddress{QHostAddress::AnyIPv4};
    std::unique_ptr<FcmAdapter> dbusAdapter;
    //QAtomicInteger<int> currNumberOfAgents{0};
    int maxNumberOfAgents{2};
    inline static QString dataBaseName{"QFcm.db"};
    inline static QString hostName;
    inline static bool dataBaseState{false};
    inline static int compression{0};
    QSqlDatabase db;
    int port{0};

    bool deleteAgent(const QString& hostName);

signals:
    void gotData(const QPair<qint32, FCM::dataFromAgent> &);
    void agentConnectedRetranslate(FCM::AgentVariant);

private slots:
    void addToDataBaseAgent(const QStringList& list);
    void justExecQuery(const QString& query);
};

#endif
