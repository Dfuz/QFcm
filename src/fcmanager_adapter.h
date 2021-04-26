#ifndef FCMANAGER_ADAPTER_H
#define FCMANAGER_ADAPTER_H

#include <QDBusAbstractAdaptor>
#include <QDBusVariant>
#include <QDBusArgument>
#include <QDBusContext>

#include "fcmanager.h"

class FCMAdapter : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", BUFFER_NAME)
    Q_PROPERTY(QString IMAGE_DATA_SHARED_ID READ imageDataSharedId)
public:
    explicit FCMAdapter(FCManager *parent);

    QString imageDataSharedId();

public slots:
    TestStructure structureField();
signals:
    void callingMe(QString, QString);
private:
    Pong * m_parentPong;

};

#endif
