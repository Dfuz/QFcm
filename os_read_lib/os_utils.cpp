#include "os_utils.h"

void OS_UTILS::OS_EVENTS::setTimer(int timer)
{
    evTimer.callOnTimeout([&]() {emit pulledOSStatus(pullOSStatus());});
    evTimer.start(timer);
}

std::tuple<OS_UTILS::FSMAP, size_t, size_t> OS_UTILS::OS_EVENTS::getFs()
{
    QProcess proc{};
    QStringList args{
        "--output=source,size,used",
        "--total",
        "-a"
    };
    qDebug()<<"starting df";
    proc.start("df", args);

    if (!proc.waitForReadyRead())
        throw std::runtime_error("failed to start process");

    proc.readLine();
    FSMAP retMap{};

    while(proc.canReadLine()) {
        auto parts = QString{proc.readLine()}.split(' ', Qt::SkipEmptyParts);
        if (parts[0].contains("total")) {
            qDebug()<<"Parsed(fs total, used): ";
            qDebug()<<parts[1].toULong();
            qDebug()<<parts[2].toULong();
            qDebug()<<"Parsed(map fs): ";
            qDebug()<<retMap;
            proc.close();
            return {retMap, parts[1].toULong(), parts[2].toULong() };
        }
        retMap.insert(parts[0], { parts[1].toULong(), parts[2].toULong() });
    }
    throw std::runtime_error("failed to parse");
}

/*!
 * \brief getPs - процессы/цпу
 * \return [Map<имя, даные процесса>, загрузка цпу в %, всего памяти, использовано памяти, кол-во процессов]
 */
std::tuple<OS_UTILS::PSMAP, float, float, float, int> OS_UTILS::OS_EVENTS::getPs()
{
    //SOME SHIT TO PARSE , INSTEAD OF . IN DOUBLE...
    QLocale c{QLocale::German};
    QProcess proc{};
    QStringList args{"-n", "1", "-b"};
    qDebug()<<"starting top";
    proc.start("top", args);
    if (!proc.waitForReadyRead())
        throw std::runtime_error("failed to start process");

    float cpuLoad, memTotal, memUsed;
    // parse until processes
    for(int n = 0; n < 7; ++n){
        if (!proc.canReadLine())
            throw std::runtime_error("failed to parse top");
        auto parts = QString{proc.readLine()}.split(' ', Qt::SkipEmptyParts);
        if (parts[0].contains("%Cpu")) cpuLoad = c.toFloat(parts[1]);
        if (parts[0].contains("MiB")   &&
            parts[1].contains("Mem")) {
            memTotal = c.toFloat(parts[3]);
            memUsed = c.toFloat(parts[7]);
        }
    }
    qDebug()<<"Parsed(cpu, mem): ";
    qDebug()<<cpuLoad;
    qDebug()<<memTotal;
    qDebug()<<memUsed;
    PSMAP retMap{};
    while(proc.canReadLine()) {
        auto parts = QString{proc.readLine()}.split(' ',Qt::SkipEmptyParts);
        retMap.insert(parts[0].toUInt(), {
            .user = parts[1],
            .command = parts[11],
            .cpu = c.toFloat(parts[8]),
            .mem = c.toFloat(parts[9])
        });
    }
    qDebug()<<"Parsed(map ps): ";
    qDebug()<<retMap;
    proc.close();
    return {retMap, cpuLoad, memTotal, memUsed, retMap.size()};
}

QDebug operator<<(QDebug debug, const OS_UTILS::PS_STRUCT &ps)
{
    QDebugStateSaver saver(debug);
    debug.space()<<"user: "<<ps.user;
    debug.space()<<"command: "<<ps.command;
    debug.space()<<"cpu: "<<ps.cpu;
    debug.space()<<"mem: "<<ps.mem;
    return debug;
}

QDebug operator<<(QDebug debug, const OS_UTILS::FS_STRUCT &fs)
{
    QDebugStateSaver saver(debug);
    debug.space()<<"total:"<<fs.total;
    debug.space()<<"used:"<<fs.used;
    return debug;
}
