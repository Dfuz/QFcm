#ifndef FCMANAGER_ADAPTER_H
#define FCMANAGER_ADAPTER_H

#include <QDBusAbstractAdaptor>
#include <QDBusVariant>
#include <QDBusArgument>
#include <QDBusContext>

#include "common/service_name_and_property.h"
#include "fcmanager.h"

class FcmAdapter : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", INTERFACE_NAME)
public:
    explicit FcmAdapter(FCManager *parent);
public slots:
    QStringList getAllAgents();
    QString getAllAgentData(const QString& hostname);
    bool deleteAgent(const QString& hostname);
signals:
    void dataJsonList(const QStringList& jsonData);
    void agentsJsonList(const QStringList& jsonData);
private:
    FCManager* m_parentManager;
};

#endif
