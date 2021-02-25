#include "os_utils.h"

void OS_UTILS::OS_EVENTS::setTimer(int timer)
{
    evTimer.callOnTimeout([&]() {emit pulledOSStatus(pullOSStatus());});
    evTimer.start(timer);
}
