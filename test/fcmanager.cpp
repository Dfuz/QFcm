#include <QtTest>
#include <QIODevice>
#include <QFile>
#include <QtConcurrent/QtConcurrent>

#include "common/utils.h"
#include "fcmanager.h"
#include "common/querybuilder.h"

class fcmanager_tests: public QObject
{
    Q_OBJECT
private slots:
    void JsonFormatTest()
    {
        QEXPECT_FAIL("", "As should be", Continue);
        QCOMPARE(Utils::JsonFormat, QSettings::InvalidFormat);
    }

    void settingsNoFile()
    {
        if (QFile::exists("conf.json")) QFile::remove("conf.json");

        FCManager manager{};
        manager.readConfig();

        QVERIFY(QFile::exists("conf.json"));

        QCOMPARE(manager.addr, QHostAddress{QHostAddress::LocalHost});
        QCOMPARE(manager.port, 1234);
        QCOMPARE(manager.maxNumberOfAgents, 4);
        QCOMPARE(manager.timeOut, std::chrono::milliseconds{0} + std::chrono::minutes{1});
    }

    void settingsFile()
    {
        QFile file{"conf.json"};
        if (file.exists()) file.open(QIODevice::Truncate | QIODevice::WriteOnly);

        const auto DATA = R"({"address":"0.0.0.0", "max_agents":10, "port":4000, "polling_rate":"1m1s"})";
        QVERIFY(file.write(DATA) != -1);
        file.close();

        FCManager manager{};
        manager.readConfig(file.fileName());

        QCOMPARE(manager.addr, QHostAddress{QHostAddress::AnyIPv4});
        QCOMPARE(manager.port, 4000);
        QCOMPARE(manager.maxNumberOfAgents, 10);
        QCOMPARE(manager.timeOut, std::chrono::milliseconds{0} + std::chrono::minutes{1} + std::chrono::seconds{1});

        QFile::remove(file.fileName());
    }

    void testMessageBuilder()
    {
        QMap<QString, QVariant> payload {
            std::pair{"test1", "testmsg"},
            std::pair{"test2", "testmsg"},
            std::pair{"test3", "testmsg"}
        };
        auto msg = Utils::TestMessage{payload};
        auto exp = QByteArray{R"({"test1":"testmsg","test2":"testmsg","test3":"testmsg","type":2})"};
        QCOMPARE(msg.toJson(), exp);

        QMap<QString, QVariant> payload2 {
            std::pair{"type", 1},
            std::pair{"test2", "testmsg"},
            std::pair{"test3", "testmsg"}
        };
        auto msg2 = Utils::ServiceMessage{payload2};
        auto exp2 = QByteArray{R"({"test2":"testmsg","test3":"testmsg","type":0})"};
        QCOMPARE(msg2.toJson(), exp2);

        auto msg3 = Utils::ServiceMessage{R"(
            {
                "field1": 1,
                "field2": 2
            }
        )"};
        auto exp3 = QByteArray{R"({"field1":1,"field2":2,"type":0})"};
        QCOMPARE(msg3.toJson(), exp3);
    }

    void testQueruBuilder()
    {
        QMap<QString, QVariant> payload {
            std::pair{"test1", "testmsg"},
            std::pair{"test2", "testmsg"},
            std::pair{"test3", "testmsg"}
        };
        auto msg = Utils::TestMessage{payload};


        QEventLoop* loop = new QEventLoop;
        qDebug() << "starting thread";
        QFuture<bool> thread = QtConcurrent::run([&]() -> bool {
            QTcpServer* server = new QTcpServer();
            server->listen(QHostAddress::LocalHost, 4001);
            qDebug() << "server: waiting connection";

            // synchronization
            if (loop)
            {
                if (loop->isRunning()) loop->quit();
                delete loop;
            }

            if(!server->waitForNewConnection(3000))
                return false;
            auto connection = server->nextPendingConnection();
            qDebug() << "server: got connection";

            auto read_data = [&]() {
                connection->waitForReadyRead();
                auto data = connection->readAll();
                qDebug()<<"server: readed data: "<<data;
            };

            auto write_data = [&]() {
                auto toSend = qCompress(msg.toJson(), 0);
                connection->write(toSend);
                connection->waitForBytesWritten();
                qDebug() << "server: sended " << toSend
                         << "\nbytes " << toSend.size();
            };

            //test1
            read_data();
            write_data();
            QThread::msleep(10);

            //test2
            write_data();
            QThread::msleep(10);

            //test3
            write_data();
            QThread::msleep(10);

            server->close();
            return true;
        });
        if (loop) loop->exec();

        QTcpSocket* sender = new QTcpSocket();
        sender->connectToHost(QHostAddress::LocalHost, 4001);
        qDebug() << "sender: waiting to connect";
        QVERIFY(sender->waitForConnected());
        qDebug() << "sender: got connection";

        auto builder = Utils::QueryBuilder{std::move(sender)};

        auto test1 = builder.makeQuery()
               .toGet<Utils::Test>()
               .toSend(msg)
               .invoke();

        QVERIFY(test1.has_value());

        auto query1 = builder.onlyGet<Utils::Test>();

        auto verificator = [&](const Utils::Message<Utils::Test> &msg) -> bool {
            qDebug() << "verificator: verifying...";
            return std::equal(msg.payload.cbegin(), msg.payload.cend(), payload.cbegin());
        };

        auto verificator_fail = [&](const Utils::Message<Utils::Test> &msg) -> bool {
            qDebug() << "verificator: verificator_failing...";
            Q_UNUSED(msg);
            return false;
        };

        query1.setVerificator(verificator);
        auto test2 = query1.invoke();
        QVERIFY(test2.has_value());

        query1.setVerificator(verificator_fail);
        auto test3 = query1.invoke();
        QVERIFY(!test3.has_value());

        sender->close();
        QVERIFY(thread.result());
    }
};

QTEST_MAIN(fcmanager_tests)
#include "fcmanager.moc"
