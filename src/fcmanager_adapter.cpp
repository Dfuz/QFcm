#include "fcmanager_adapter.h"

FCMAdapter::FCMAdapter(FCManager *parent):
    QDBusAbstractAdaptor(parent)
{
    this->m_parentManager = parent;
}

QStringList FCMAdapter::getAllAgents()
{
    return m_parentManager->getAllAgents();
}

QStringList FCMAdapter::getAllAgentData(const QString &hostname)
{
    return m_parentManager->getAllAgentData(hostname);
}
