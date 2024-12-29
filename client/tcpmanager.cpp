#include "tcpmanager.h"
#include "usermanager.h"
#include "ordersmanager.h"

#include <QDataStream>
#include <QAbstractSocket>

TcpManager::~TcpManager()
{
    qDebug() << "Tcpmanager has been destructed!";
}

TcpManager::TcpManager()
    : _host(""), _port(0), _message_id(0), _message_lenghth(0), _b_recv_pending(false)
{
    QObject::connect(&_socket, &QTcpSocket::connected, [&]
    {
        qDebug() << "Connected to server!";
        emit sig_con_success(true);
    });

    QObject::connect(&_socket, &QTcpSocket::readyRead, [&]
    {
        _buffer.append(_socket.readAll());

        QDataStream stream(&_buffer, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_5_1);

        forever
        {
            // 头部没解析完
            if(!_b_recv_pending)
            {
                if(_buffer.size() < static_cast<int>(sizeof(qint16) * 2))
                    return;

                stream >> _message_id >> _message_lenghth;

                _buffer = _buffer.mid(sizeof(qint16) * 2);

                qDebug() << "message id is: " << _message_id << " message len is: " << _message_lenghth;
            }

            // 开始接收包体
            if(_buffer.size() < _message_lenghth)
            {
                _b_recv_pending = true;
                return;
            }

            _b_recv_pending = false;
            QByteArray messageBody = _buffer.mid(0, _message_lenghth);
            qDebug() << "message is: " << messageBody;

            _buffer = _buffer.mid(_message_lenghth);
            handleMessage(ReqId(_message_id), _message_lenghth, messageBody);
        }
    });

    // 处理错误
    QObject::connect(&_socket, static_cast<void(QTcpSocket::*)(QTcpSocket::SocketError)>(&QTcpSocket::error), [&](QTcpSocket::SocketError socketError)
    {
        qDebug() << "Error:" << _socket.errorString();
        switch (socketError)
        {
        case QTcpSocket::ConnectionRefusedError:
            qDebug() << "Connection Refused!";
            emit sig_con_success(false);
            break;
        case QTcpSocket::RemoteHostClosedError:
            qDebug() << "Remote Host Closed Connection!";
            break;
        case QTcpSocket::HostNotFoundError:
            qDebug() << "Host Not Found!";
            emit sig_con_success(false);
            break;
        case QTcpSocket::SocketTimeoutError:
            qDebug() << "Connection Timeout!";
            emit sig_con_success(false);
            break;
        case QTcpSocket::NetworkError:
            qDebug() << "Network Error!";
            break;
        default:
            qDebug() << "Other Error!";
            break;
        }
    });

    QObject::connect(&_socket, &QTcpSocket::disconnected, [&]()
    {
        qDebug() << "Disconnected from server";
    });

    QObject::connect(this, &TcpManager::sig_send_data, this, &TcpManager::slot_send_data);

    initHandlers();
}

void TcpManager::initHandlers()
{
    _handlers.insert(ReqId::ID_CHAT_LOGIN_RSP, [this](ReqId id, int len, QByteArray data)
    {
        Q_UNUSED(len);
        qDebug() << "message id is: " << id << " message len is: " << len;

        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        if(jsonDoc.isNull())
        {
            qDebug() << "Failed to create QJsonDocument";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        // 随便设置一个判断有没有json
        if(!jsonObj.contains("error"))
        {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Login failed, err is Json Parse Err";
            emit sig_login_failed(err);
            return;
        }

        int err = jsonObj["error"].toInt();
        if(err != ErrorCodes::SUCCESS)
        {
            qDebug() << "Login failed, err is: " << err;
            emit sig_login_failed(err);
            return;
        }

        // 存储用户数据
        UserManager::GetInstance()->SetUserLevel(static_cast<UserLevel>(random() % 3));
        UserManager::GetInstance()->SetUid(jsonObj["uid"].toInt());
        UserManager::GetInstance()->SetName(jsonObj["name"].toString());
        UserManager::GetInstance()->SetToken(jsonObj["token"].toString());
        UserManager::GetInstance()->SetHead(jsonObj["head"].toString());
        UserManager::GetInstance()->SetEmail(jsonObj["email"].toString());

        emit sig_switch_chatdialog();
    });

    _handlers.insert(ReqId::ID_PURCHASE_RSP, [this](ReqId id, int len, QByteArray data)
    {
        Q_UNUSED(len);
        qDebug() << "message id is: " << id << " message len is: " << len;

        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        qDebug() << "jsonDoc is: " << jsonDoc;
        QJsonObject jsonObj = jsonDoc.object();

        if(!jsonObj.contains("error"))
        {
            qDebug() << "Failed to create QJsonDocument";
            return;
        }
    });

    _handlers.insert(ReqId::ID_GET_ORDERS_RSP, [this](ReqId id, int len, QByteArray data)
    {
        Q_UNUSED(len);
        qDebug() << "message id is: " << id << " message len is: " << len;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        qDebug() << "jsonDoc is: " << jsonDoc;
        
        QJsonObject jsonObj = jsonDoc.object();
        if(!jsonObj.contains("order_id"))
        {
            qDebug() << "Failed to create QJsonDocument";
            return;
        }
        OrdersManager::GetInstance()->AddOrder(jsonObj);
    });
}

void TcpManager::handleMessage(ReqId id, int len, QByteArray body)
{
    if(_handlers.contains(id))
        _handlers[id](id, len, body);
    else
        qDebug() << "id: " << id << "not has handler";
}

// connect to ChatServer
void TcpManager::slot_tcp_connect(ServerInfo si)
{
    _host = si.Host;
    _port = static_cast<std::uint16_t>(si.Port.toUInt());
    _socket.connectToHost(si.Host, _port);
}

void TcpManager::slot_send_data(ReqId reqid, QString body)
{
    std::uint16_t id = reqid;

    QByteArray dataBytes = body.toUtf8();
    
    quint16 len = static_cast<quint16>(dataBytes.size());

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);

    // 写入TLV
    out << id << len;
    block.append(dataBytes);

    qDebug() << "Sending data with length:" << len << "bytes";
    qDebug() << "Actual data:" << dataBytes;

    _socket.write(block);
}
