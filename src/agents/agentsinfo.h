#ifndef AGENTSINFO_H
#define AGENTSINFO_H

#include <QTimer>
#include <variant>

namespace FCM {

struct Agent
{
    QTimer timer;
};

struct Proxy
{
    std::map<qint32, Agent> agents;
};

using AgentVariant = std::variant<Agent, Proxy>;

}

#endif // AGENTSINFO_H
