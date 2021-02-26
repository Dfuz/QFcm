#ifndef FCMTHREADPOLLER_H
#define FCMTHREADPOLLER_H

#include <QThread>
#include <QObject>

/*!
 * \brief The FcmThreadPoller class - уже поток на каждого агента для вытягивания даты
 * (цпу, память и тд...)
 */
class FcmThreadPoller : public QThread
{
    Q_OBJECT
public:
    FcmThreadPoller();
};

#endif // FCMTHREADPOLLER_H
