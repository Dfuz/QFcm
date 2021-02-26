#ifndef FCMTHREADHANDSHAKE_H
#define FCMTHREADHANDSHAKE_H

#include <QThread>
#include <QObject>

/*!
 * \brief The FcmThreadHandshake class - используем для "рукопожатия"
 * (удостоверимся что агент, мб занесем в базу данных)
 */
class FcmThreadHandshake : public QThread
{
    Q_OBJECT
public:
    FcmThreadHandshake();
};

#endif // FCMTHREADHANDSHAKE_H
