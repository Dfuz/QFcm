#include "os_utils.h"

template<bool full>
std::tuple<OS_UTILS::FSMAP, size_t, size_t> OS_UTILS::OS_EVENTS<full>::getFs()
{
    QProcess proc{};
    QStringList args{
        "--output=source,size,used",
        "--total",
        "-a"
    };
    proc.start("df", args);

    if (!proc.waitForReadyRead())
        throw std::runtime_error("failed to start process");

    proc.readLine();
    FSMAP retMap{};

    while(proc.canReadLine()) {
        QString line_s{proc.readLine()};
        auto parts = line_s.split(' ');
        if (parts[0].contains("total"))
            return {retMap, parts[1].toULong(), parts[2].toULong() };
        retMap.insert(parts[0], { parts[1].toULong(), parts[2].toULong() });
    }
}

/*!
 * \brief getPs - процессы/цпу
 * \return [Map<имя, даные процесса>, загрузка цпу в %, всего памяти, использовано памяти, кол-во процессов]
 */
template<bool full>
std::tuple<OS_UTILS::PSMAP, float, float, float, int> OS_UTILS::OS_EVENTS<full>::getPs()
{
    QProcess proc{};
    QStringList args{"-n", "1", "-b"};
    proc.start("top", args);
    if (!proc.waitForReadyRead())
        throw std::runtime_error("failed to start process");

    float cpuLoad, memTotal, memUsed;
    // parse until processes
    while(proc.canReadLine()) {
        auto line = proc.readLine();
        if (line.isEmpty()) break;
        auto line_s = QString{line};

    }
    proc.readLine();
    PSMAP retMap{};
    while(proc.canReadLine()) {

    }
}
