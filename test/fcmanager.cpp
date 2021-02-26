#include <QtTest>
#include <QIODevice>
#include <QFile>
#include "utils.h"
#include "fcmanager.h"

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

        auto config = manager.getConfig();

        QCOMPARE(config.addr, QHostAddress{QHostAddress::LocalHost});
        QCOMPARE(config.port, 1234);
        QCOMPARE(config.maxNumberOfAgents, 4);
        QCOMPARE(config.timeOut, std::chrono::milliseconds{0} + std::chrono::minutes{1});
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
        auto config = manager.getConfig();

        QCOMPARE(config.addr, QHostAddress{QHostAddress::AnyIPv4});
        QCOMPARE(config.port, 4000);
        QCOMPARE(config.maxNumberOfAgents, 10);
        QCOMPARE(config.timeOut, std::chrono::milliseconds{0} + std::chrono::minutes{1} + std::chrono::seconds{1});

        QFile::remove(file.fileName());
    }
};

QTEST_MAIN(fcmanager_tests)
#include "fcmanager.moc"
