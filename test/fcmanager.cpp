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
        QTcpSocket* sender = new QTcpSocket();
        //FIXME: как то протестить сокет
        sender->bind(4000);
        auto builder = Utils::QueryBuilder{std::move(sender)};
        QTcpSocket reciver;
        reciver.bind(5000);

        QMap<QString, QVariant> payload {
            std::pair{"test1", "testmsg"},
            std::pair{"test2", "testmsg"},
            std::pair{"test3", "testmsg"}
        };
        auto msg = Utils::TestMessage{payload};

        auto write_data = [&](bool compress = false) {
            reciver.write(compress ? qCompress(msg.toJson()) : msg.toJson());
        };

        auto read_data = [&]() {
            reciver.waitForReadyRead();
            return qUncompress(reciver.readAll());
        };

        Q_UNUSED(read_data);

//        Должны тестить в отдельном потоке...
//        auto test1 = builder.makeQuery()
//               .toGet<Utils::Test>()
//               .toSend(msg)
//               .invoke();

//        read_data();
//        write_data();

        auto query1 = builder.onlySend(msg);


        auto query2 = builder.onlyGet<Utils::Test>();

        auto verificator = [&](const Utils::Message<Utils::Test> &msg) -> bool {
            return std::equal(msg.payload.cbegin(), msg.payload.cend(), payload.cbegin());
        };

        auto verificator_fail = [&](const Utils::Message<Utils::Test> &msg) -> bool {
            Q_UNUSED(msg);
            return false;
        };

        query2.setVerificator(verificator);
        write_data();
        auto test4 = query2.invoke();
        QVERIFY(test4.has_value());

        query2.setVerificator(verificator_fail);
        write_data();
        auto test5 = query2.invoke();
        QVERIFY(!test5.has_value());

//        query1.setVerificator(verificator);
//        НЕ сработает так как не совпадает по типам

    }
};

QTEST_MAIN(fcmanager_tests)
#include "fcmanager.moc"
