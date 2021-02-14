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
        FCManager manager{};
        manager.readConfig();

        QFile file{manager.settings_path};

        QVERIFY(file.exists());

        QCOMPARE(manager.addr(), QHostAddress{QHostAddress::LocalHost});
        QCOMPARE(manager.port(), 1234);
        QCOMPARE(manager.max_number_of_agents(), 4);
    }

    void settingsFile()
    {
        QFile file{"conf.json"};
        if (file.exists()) file.open(QIODevice::Truncate | QIODevice::WriteOnly);

        const auto DATA = R"({"address":"0.0.0.0", "max_agents":10, "port":4000})";
        QVERIFY(file.write(DATA) != -1);
        file.close();

        FCManager manager{};
        manager.settings_path = file.fileName();
        manager.readConfig();

        QCOMPARE(manager.addr(), QHostAddress{QHostAddress::AnyIPv4});
        QCOMPARE(manager.port(), 4000);
        QCOMPARE(manager.max_number_of_agents(), 10);

        QFile::remove(file.fileName());
    }
};

QTEST_MAIN(fcmanager_tests)
#include "fcmanager.moc"
