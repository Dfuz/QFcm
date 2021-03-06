#ifndef QUERYBUILDER_H
#define QUERYBUILDER_H

#include <QTcpSocket>
#include <QDataStream>
#include <QBuffer>
#include <algorithm>
#include <memory>
#include <type_traits>
#include <variant>
#include <functional>

#include "messagebuilder.h"

namespace Utils {

enum QueryDirection {
    Bidirectional,
    Unidirectional
};

template<MessageType ret>
using Verificator = std::function<bool(const Message<ret> &)>;

template<QueryDirection direct, MessageType to = NoMessage, MessageType ret = NoMessage>
struct Query {

    template<MessageType invRet>
    using InvokeReturn = std::optional<Message<invRet>>;

    Query(std::shared_ptr<QTcpSocket> _socket):
        socket(_socket)
    {}
    Query(std::shared_ptr<QTcpSocket> _socket, const Message<to> _msg):
        socket(_socket), msg(_msg)
    {}

    /*!
     * \brief toSend - задаем сообщение для отправки
     * \param _msg - сообщение Message<newTo>
     * \param compressionLevel - уровень компресии 0-9, -1 - zlib
     * \return
     */
    template<MessageType newTo>
    constexpr Query<direct, newTo, ret> toSend(const Message<newTo> & _msg, int newCompressionLevel = 0) noexcept {
        Query<direct, newTo, ret> retvar{socket, _msg};
        return retvar.setCompression(newCompressionLevel);
    };

    /*!
     * \brief toSend - задаем сообщение которое ожидаем
     * \param _msg - сообщение Message<newRet>
     * \param uncompression - нужно ли декомпрессировать
     * \return
     */
    template<MessageType newRet>
    constexpr Query<direct, to, newRet> toGet() noexcept {
        return {socket, msg};
    };

    /*!
     * \brief invoke - вызов сессии связи
     * \return
     */
    template<QueryDirection T = direct,
             std::enable_if_t<T == Bidirectional, bool> = true>
    InvokeReturn<ret> invoke() noexcept {

        // Участок отправки сообщения
        if constexpr (to != NoMessage) {
            auto toSend = qCompress(msg.toJson(), compressionLevel);
            writeMessage(toSend);
            if (!socket->waitForBytesWritten()) {
                qDebug()<<"Query: failed to write";
                return std::nullopt;
            }
        }

        // Участок приема сообщения
        if constexpr (ret == NoMessage)
            return Message<NoMessage>{};

        qDebug()<<"Query: waiting to read";

        if (!socket->waitForReadyRead())
            return std::nullopt;

        auto gotRaw = readMessage();

        auto got = Message<ret>::parseJson(qUncompress(gotRaw));

        if(!got.has_value()) {
            qDebug()<<"Query: failed to parse: "<<gotRaw.data();
            return std::nullopt;
        }

        // Проходим по верификаторам
        if (!std::all_of(verificators.cbegin(), verificators.cend(),
                        [&](auto fn) { return fn(got.value());}))
            return std::nullopt;

        return got;
    }

    /*!
     * \brief invoke - вызов сессии связи
     * (Вариант для Unidirectional)
     * \return
     */
    template<QueryDirection T = direct,
             std::enable_if_t<T == Unidirectional, bool> = true>
    Message<NoMessage> invoke() noexcept {

        if constexpr (to != NoMessage) {
            auto toSend = qCompress(msg.toJson(), compressionLevel);
            socket->waitForBytesWritten();
        }

        qDebug()<<"Query: sended";

        return {};
    }

    /*!
     * \brief setVerificator
     * \param verFn - верификатор по которому можно проверить сообщение
     * \return
     */
    constexpr Query& setVerificator(Verificator<ret> verFn) noexcept {
        verificators.push_back(verFn);
        return *this;
    }
    /*!
     * \brief setCompression - задаем уровень компрессии
     * \param newLevel
     * \return
     */
    constexpr Query& setCompression(int newLevel) noexcept {
        compressionLevel = newLevel;
        return *this;
    }

    friend class fcmanager_tests;

private:
    std::shared_ptr<QTcpSocket> socket;
    Message<to> msg;
    std::vector<Verificator<ret>> verificators;
    int compressionLevel = 0;

    void writeMessage(const QByteArray &toSend) {
        socket->write(toSend);
        qDebug()<<"Query: sended "<<toSend<<"\nbytes "<<toSend.size();
    }

    QByteArray readMessage() {
        QByteArray retval = socket->readAll();
        qDebug()<<"Query: readed "<<retval;
        return retval;
    }
};

struct QueryBuilder {
public:
    QueryBuilder(): socket() {}
    QueryBuilder(QTcpSocket *skt): socket(skt) {}
    QueryBuilder(qintptr ptrskt) noexcept {
        QTcpSocket* skt = new QTcpSocket();
        skt->setSocketDescriptor(ptrskt);
        socket = std::make_shared<QTcpSocket>(skt);
    }

    bool setSocketDescriptor(qintptr ptrskt) {
        return socket->setSocketDescriptor(ptrskt);
    }

    /*!
     * \brief makeQuery - создание сесси связи
     * \return
     */
    template<QueryDirection direct = Bidirectional>
    Query<direct> makeQuery() noexcept {
        return {socket};
    };

    /*!
     * \brief onlySend - вариант только отправки
     * \param msg
     * \return
     */
    template<MessageType to>
    Query<Unidirectional, to, NoMessage> onlySend(const Message<to> & msg) noexcept {
        return makeQuery<Unidirectional>()
                .toGet<NoMessage>()
                .toSend<to>(msg);
    }

    /*!
     * \brief onlyGet - вариант только получения
     * \return
     */
    template<MessageType ret>
    Query<Bidirectional, NoMessage, ret> onlyGet() noexcept {
        return makeQuery<Bidirectional>()
                .toSend<NoMessage>({})
                .toGet<ret>();
    }

private:
    std::shared_ptr<QTcpSocket> socket;
};

}

#endif // QUERYBUILDER_H
