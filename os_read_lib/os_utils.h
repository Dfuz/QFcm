#ifndef OS_UTILS_H
#define OS_UTILS_H

#ifndef __linux
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
    float cpu;
    float mem;
};

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

    float cpuLoad;
    int psCount;

    float MemoryTotal;
    float MemoryUsed;

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
        const auto [allFs, TotalFSSize, UsedFSSize] = getFs();
        const auto [allPs, cpuLoad, MemoryTotal, MemoryUsed, psCount] = getPs();
        return {
            .TotalFSSize = TotalFSSize,
            .UsedFSSize = UsedFSSize,
            .cpuLoad = cpuLoad,
            .psCount = psCount,
            .MemoryTotal = MemoryTotal,
            .MemoryUsed = MemoryUsed,
            .allFs = allFs,
            .allPs = allPs
        };
    }

signals:
    void pulledOSStatus(OS_STATUS);

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
