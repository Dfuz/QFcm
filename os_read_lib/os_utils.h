#ifndef OS_UTILS_H
#define OS_UTILS_H

#ifndef __linux
#error "curenntly unsupported"
#endif

#include <utils.h>
#include <stdexcept>
#include <QMap>
#include <QObject>
#include <type_traits>
#include <QProcess>
#include <QTimer>

namespace OS_UTILS
{

struct OS_STATUS
{
    size_t TotalFSSize;
    size_t UsedFSSize;

    float cpuLoad;
    int psCount;

    float MemoryTotal;
    float MemoryUsed;

//    QString to_json();
};

/*!
 * \brief The PS_STRUCT struct - цпу и память в процентах по всем процессам
 */
struct PS_STRUCT
{
    QString user;
    int cpu;
    int mem;
};

struct FS_STRUCT
{
    size_t total;
    size_t used;
};

typedef QMap<QString, FS_STRUCT> FSMAP;
typedef QMap<QString, PS_STRUCT> PSMAP;

struct OS_STATUS_FULL : public OS_STATUS
{
    FSMAP allFs;
    PSMAP allPs;
};

/*!
  \class OS_EVENTS
  \brief класс для вытягивания данных OS (пока только линукс)
 */
template<bool full = false>
class OS_EVENTS : public QObject
{
    typedef typename std::conditional<full, OS_STATUS_FULL, OS_STATUS>::type STATUS;

    Q_OBJECT
public:
    explicit OS_EVENTS(QObject *parent = nullptr);

    void setTimer(int timer = 1000)
    {
        evTimer.callOnTimeout([&]() {emit pulledOSStatus(pullOSStatus());});
        evTimer.start(timer);
    }

    void stopTimer() {evTimer.stop();};

    /*!
     * \brief pullOSStatus - Вытягивает статуи системы
     * \return Статус системы
     */
    static STATUS pullOSStatus()
    {
        const auto [allFs, TotalFSSize, UsedFSSize] = getFs();
        const auto [allCpu, cpuLoad, MemoryTotal, MemoryUsed, psCount] = getPs();
        return {
            TotalFSSize,
            UsedFSSize,
            psCount,
            cpuLoad,
            MemoryTotal,
            MemoryUsed,
            allCpu,
            allFs
        };
    }

signals:
    void pulledOSStatus(STATUS);

private:
    QTimer evTimer{this};

    /*!
     * \brief getFs - файловая система
     * \return [Map<имя, фс данные> всего доступно, использовано]
     */
    static std::tuple<FSMAP, size_t, size_t> getFs();
    /*!
     * \brief getPs - процессы/цпу
     * \return [Map<имя, даные процесса>, загрузка цпу в %, всего памяти, использовано памяти, кол-во процессов]
     */
    static std::tuple<PSMAP, float, float, float, int> getPs();
};

}

#endif // OS_UTILS_H
