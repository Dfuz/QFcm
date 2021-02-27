#include "fcm_thread.h"

FcmThread::FcmThread(qintptr ID, QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket();

    if(!socket->setSocketDescriptor(ID))
        throw std::runtime_error("Something went wrong with socket");
}

void FcmThread::doSomeWork()
{
    qDebug() << "Поток запущен!";

    connect(socket, &QTcpSocket::readyRead, this, &FcmThread::readyRead, Qt::DirectConnection);
    connect(socket, &QTcpSocket::disconnected, this, &FcmThread::disconnected);

    qDebug() << socket->socketDescriptor() << " Client connected";

    emit resultReady(QString("Result"));
}

std::optional<FCM::AgentVariant> FcmThread::performHandshake()
{
    return std::nullopt;
}

void FcmThread::readyRead()
{
    QByteArray Data = socket->readAll();
    qDebug() << socket->socketDescriptor() << " Data in: " << Data;
    socket->write(Data);
}

void FcmThread::disconnected()
{
    qDebug() << socket->socketDescriptor() << " Disconnected";
    socket->deleteLater();
    exit(0);
}
