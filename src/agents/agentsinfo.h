#ifndef AGENTSINFO_H
#define AGENTSINFO_H

#include <QTimer>
#include <variant>

class AgentInterface
{
    QTimer pollingRate{};   // Deafaulted from constructer
};

class Agent: public AgentInterface
{

};

class Proxy: public AgentInterface
{

};

typedef std::variant<Agent, Proxy> AgentVariant;

#endif // AGENTSINFO_H
