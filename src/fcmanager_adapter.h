#ifndef FCMANAGER_ADAPTER_H
#define FCMANAGER_ADAPTER_H

#include <QDBusAbstractAdaptor>
#include <QDBusVariant>
#include <QDBusArgument>
#include <QDBusContext>

#include "common/service_name_and_property.h"
#include "fcmanager.h"

class FCMAdapter : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", INTERFACE_NAME)
public:
    explicit FCMAdapter(FCManager *parent);
public slots:
    QStringList getAllAgents();
    QStringList getAllAgentData(const QString& hostname);
signals:
    void queryWithAgentData(const QStringList& sqlQuerys);
private:
    FCManager* m_parentManager;
};

#endif
