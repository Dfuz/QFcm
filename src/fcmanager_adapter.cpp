#include "fcmanager_adapter.h"

FcmAdapter::FcmAdapter(FCManager *parent):
    QDBusAbstractAdaptor(parent)
{
    this->m_parentManager = parent;
}

QStringList FcmAdapter::getAllAgents()
{
    return m_parentManager->getAllAgents();
}

QString FcmAdapter::getAllAgentData(const QString &hostname)
{
    return m_parentManager->getAllAgentData(hostname);
}

bool FcmAdapter::deleteAgent(const QString& hostname)
{
    return m_parentManager->deleteAgent(hostname);
}
