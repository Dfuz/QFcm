#ifndef AGENTSINFO_H
#define AGENTSINFO_H

#include <QTimer>
#include <variant>
#include <QMap>

namespace FCM {

/*!
 * \brief The PS_STRUCT struct - цпу и память в процентах по всем процессам
 */
struct PS_STRUCT
{
    QString user;
    QString command;
    uint cpu;
    long mem;
};

/*!
 * \brief The FS_STRUCT struct - тип память по всем файл системам
 */
struct FS_STRUCT
{
    QString type;
    ulong total;
    ulong free;
};

typedef QMap<QString, FS_STRUCT> FSMAP;
typedef QMap<uint, PS_STRUCT> PSMAP;

struct OS_STATUS
{
    size_t TotalFSSize;
    size_t FreeFSSize;

    ulong cpuLoad;
    ushort psCount;

    ulong MemoryTotal;
    ulong MemoryFree;

    FSMAP allFs;
    PSMAP allPs;
};

struct Agent
{
    QString hostName;
    QString macAddress;
   //OS_STATUS status;

    /*void clearData(void)
    {
        status.allFs.clear();
        status.allPs.clear();
    }*/
};

struct Proxy
{
    QString hostName;
    QString macAddress;
    //OS_STATUS status;

    /*void clearData(void)
    {
        status.allFs.clear();
        status.allPs.clear();
    }*/
};

using AgentVariant = std::variant<Agent, Proxy>;

}

Q_DECLARE_METATYPE(FCM::AgentVariant);

#endif // AGENTSINFO_H
