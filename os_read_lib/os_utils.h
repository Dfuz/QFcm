#ifndef OS_UTILS_H
#define OS_UTILS_H

#ifdef __linux
#include <linux/sysinfo.h>
#include <sys/sysinfo.h>

#include <proc/readproc.h>

#include <libmount/libmount.h>
#else
#error "curenntly unsupported"
#endif

//#include <utils.h>
#include <tuple>
#include <stdexcept>
#include <QMap>
#include <QObject>
#include <type_traits>
#include <QProcess>
#include <QTimer>
#include <QLocale>
#include <QDebug>

namespace OS_UTILS
{

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

QDebug operator<<(QDebug debug, const OS_UTILS::PS_STRUCT &ps)
{
    QDebugStateSaver saver(debug);
    debug.space()<<"user: "<<ps.user;
    debug.space()<<"command: "<<ps.command;
    debug.space()<<"cpu: "<<ps.cpu;
    debug.space()<<"mem: "<<ps.mem;
    return debug;
}

struct FS_STRUCT
{
    size_t total;
    size_t used;
};

typedef QMap<QString, FS_STRUCT> FSMAP;
typedef QMap<uint, PS_STRUCT> PSMAP;

struct OS_STATUS
{
    size_t TotalFSSize;
    size_t UsedFSSize;

    ulong cpuLoad;
    ushort psCount;

    ulong MemoryTotal;
    ulong MemoryFree;

    FSMAP allFs;
    PSMAP allPs;
};

/*!
  \class OS_EVENTS
  \brief класс для вытягивания данных OS (пока только линукс)
 */
class OS_EVENTS : public QObject
{
    Q_OBJECT
public:
    explicit OS_EVENTS(QObject *parent = nullptr);

    void setTimer(int timer = 1000);

    void stopTimer() {evTimer.stop();};

    /*!
     * \brief pullOSStatus - Вытягивает статуи системы
     * \return Статус системы
     */
    static OS_STATUS pullOSStatus()
    {
        struct sysinfo si;
        if (sysinfo(&si) == -1)
            throw std::runtime_error("failed to read sysinfo()");

        auto procs = readproctab(PROC_FILLUSR | PROC_FILLSTAT | PROC_FILLSTATUS | PROC_FILLCOM | PROC_FILLMEM);
        if (procs == NULL)
            throw std::runtime_error("failed to read procs");

        PSMAP allPs{};
        for(int i = 0; procs[i] != NULL; ++i)
            allPs.insert(procs[i]->tid, {
                             QString{procs[i]->ruser},
                             QString{procs[i]->cmd},
                             procs[i]->pcpu,    //FIXME: count cpu by ->starttime/utime... mb...
                             procs[i]->size
                         });

        //TODO: get FS
        size_t TotalFSSize = 0, UsedFSSize = 0;
        FSMAP allFs{};
        //TODO: read fs info from here
        qDebug()<<_PATH_MOUNTED;
        qDebug()<<allPs;

        return {
            .TotalFSSize = TotalFSSize,
            .UsedFSSize = UsedFSSize,

            .cpuLoad = si.loads[0],
            .psCount = si.procs,
            .MemoryTotal = si.totalram,
            .MemoryFree = si.freeram,

            .allFs = allFs,
            .allPs = allPs
        };
    }

signals:
    void pulledOSStatus(OS_STATUS);

private:
    QTimer evTimer{this};
};

}

#endif // OS_UTILS_H
