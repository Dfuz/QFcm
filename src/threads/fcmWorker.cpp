#include "fcmWorker.h"

FcmWorker::FcmWorker(qintptr ID, QObject *parent) : QObject(parent)
{
//    socket = new QTcpSocket();

//    if(!socket->setSocketDescriptor(ID))
//        throw std::runtime_error("Something went wrong with socket");
}

void FcmWorker::doSomeWork()
{
//    qDebug() << "Поток запущен!";

//    connect(socket, &QTcpSocket::readyRead, this, &FcmWorker::readyRead, Qt::DirectConnection);
//    connect(socket, &QTcpSocket::disconnected, this, &FcmWorker::disconnected);

//    qDebug() << socket->socketDescriptor() << " Client connected";

//    emit finished();
}

std::optional<FCM::AgentVariant> FcmWorker::performHandshake()
{
    return std::nullopt;
}

void FcmWorker::readyRead()
{
//    QByteArray Data = socket->readAll();
//    qDebug() << socket->socketDescriptor() << " Data in: " << Data;
//    socket->write(Data);
}

void FcmWorker::disconnected()
{
//    qDebug() << socket->socketDescriptor() << " Disconnected";
//    socket->deleteLater();
//    exit(0);
}
