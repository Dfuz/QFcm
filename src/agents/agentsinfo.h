#ifndef AGENTSINFO_H
#define AGENTSINFO_H

#include <QTimer>
#include <variant>

namespace FCM {

struct Agent
{

};

struct Proxy
{

};

using AgentVariant = std::variant<Agent, Proxy>;

}

#endif // AGENTSINFO_H
