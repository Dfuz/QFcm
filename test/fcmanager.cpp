#include <QtTest>
#include <QIODevice>
#include <QFile>

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

    void testQueruBuilder()
    {
        auto builder = Utils::QueryBuilder{new QTcpSocket()};

        QMap<QString, QVariant> payload {
            std::pair{"test1", "testmsg"},
            std::pair{"test2", "testmsg"},
            std::pair{"test3", "testmsg"}
        };

        auto msg = Utils::TestMessage{payload};
        auto test1 = builder.makeQuery()
                .toSend(msg)
                .toGet<Utils::Data>();

        auto test2 = builder.makeQuery()
               .toGet<Utils::Data>()
               .toSend(msg)
               .invoke();

        auto test3 = builder.makeQuery<Utils::Unidirectional>()
               .toSend(msg)
               .toGet<Utils::Test>()
               .invoke();

        auto query1 = builder.onlySend(msg);
        auto query2 = builder.onlyGet<Utils::Test>();

        query1.setVerificator([](const auto &msg) {
            Q_UNUSED(msg);
            return true;
        });

        auto verificator = [](const Utils::Message<Utils::Test> &msg) -> bool {
            Q_UNUSED(msg);
            return true;
        };

        query2.setVerificator(verificator);
        auto test4 = query2.invoke();
        if (!test4.has_value()) qInfo()<<"got none";

//        query1.setVerificator(verificator);
//        НЕ сработает так как не совпадает по типам, я на хайпееееее...

    }
};

QTEST_MAIN(fcmanager_tests)
#include "fcmanager.moc"
