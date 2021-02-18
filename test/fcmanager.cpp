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

        QVERIFY(QFile::exists(manager.settings_path));

        QCOMPARE(manager.getAddr(), QHostAddress{QHostAddress::LocalHost});
        QCOMPARE(manager.getPort(), 1234);
        QCOMPARE(manager.get_max_number_of_agents(), 4);
        QCOMPARE(manager.getTimeOut(), std::chrono::milliseconds{0} + std::chrono::minutes{1});
    }

    void settingsFile()
    {
        QFile file{"conf.json"};
        if (file.exists()) file.open(QIODevice::Truncate | QIODevice::WriteOnly);

        const auto DATA = R"({"address":"0.0.0.0", "max_agents":10, "port":4000, "polling_rate":"1m1s"})";
        QVERIFY(file.write(DATA) != -1);
        file.close();

        FCManager manager{};
        manager.settings_path = file.fileName();
        manager.readConfig();

        QCOMPARE(manager.getAddr(), QHostAddress{QHostAddress::AnyIPv4});
        QCOMPARE(manager.getPort(), 4000);
        QCOMPARE(manager.get_max_number_of_agents(), 10);
        QCOMPARE(manager.getTimeOut(), std::chrono::milliseconds{0} + std::chrono::minutes{1} + std::chrono::seconds{1});

        QFile::remove(file.fileName());
    }
};

QTEST_MAIN(fcmanager_tests)
#include "fcmanager.moc"
