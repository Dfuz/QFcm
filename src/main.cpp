#include <QCommandLineParser>
#include <QDBusConnection>
#include <QDBusError>
#include "fcmanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication application(argc, argv);
    QCoreApplication::setApplicationName("FC Manager");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    parser.addOption({{"n", "name"},        "Set hostname",    "value"});
    parser.addOption({{"c", "compression"}, "Set compression", "value"});
    parser.addOption({{"f", "db"},          "Set data base file name", "value"});
    parser.addOption({{"j", "threads"},     "Set max thread count",    "value"});
    parser.addOption({{"p", "port"},        "Set port",    "value"});
    parser.addOption({{"a", "address"},     "Set address",    "value"});
    parser.addOption({{"s", "settings"},    "Set JSON configuration file",    "value"});
    parser.process(application);

    FCManager server(&application);

    if (parser.value("settings").isEmpty())
        server.readConfig();
    else server.readConfig(parser.value("settings"));

    if (!parser.value("name").isEmpty())
        server.setHostName(parser.value("name"));
    if (!parser.value("port").isEmpty())
        server.setPort(parser.value("port").toInt());
    if (!parser.value("db").isEmpty())
        server.setDBFile(parser.value("db"));
    if (!parser.value("threads").isEmpty())
        server.setMaxNumberOfAgents(parser.value("threads").toInt());
    if (!parser.value("address").isEmpty())
        server.setIpAddress(parser.value("address"));
    if (!parser.value("compression").isEmpty())
        server.setCompression(parser.value("compression").toInt());

    server.initDBConnection();
    if (!server.startServer())
        application.exit(-1);

    QDBusConnection connection = QDBusConnection::sessionBus();
    if(!connection.registerObject("/", &server)){
        qDebug() << "Can't register object" << QDBusConnection::sessionBus().lastError().message();
        application.exit(1);
    }
    if (!connection.registerService(SERVICE_NAME)) {
        qDebug() << "Can't register service" << QDBusConnection::sessionBus().lastError().message();
        application.exit(1);
    }

    return application.exec();
}
