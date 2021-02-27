#include "fcm_thread.h"

FcmThread::FcmThread(qintptr ID, QObject *parent) :
    QThread(parent)
{
    this->socketDescriptor = ID;
}

void FcmThread::run()
{
    qDebug() << "Поток запущен!";

    socket = new QTcpSocket();

    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        // что-то пошло не так, объект потока вырабатывает сигнал ошибки
        // FIXME: сделать обработчик этой ошибки
        emit error(socket->error());
        return;
    }

    // connect socket and signal
    // Qt::DirectConnection is used because it's multithreaded
    // this makes the slot to be invoked immediately, when the signal is emitted.
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));


    // We'll have multiple clients, we want to know which is which
    qDebug() << socketDescriptor << " Client connected";

    // Какой нить handshake
    // Если все окк то parent->addAgent();
    // иначе disconnect()

    exec();
}

void FcmThread::readyRead()
{
    // get the information
    QByteArray Data = socket->readAll();

    // will write on server side window
    qDebug() << socketDescriptor << " Data in: " << Data;

    socket->write(Data);
}

void FcmThread::disconnected()
{
    qDebug() << socketDescriptor << " Disconnected";
    socket->deleteLater();
    exit(0);
}
