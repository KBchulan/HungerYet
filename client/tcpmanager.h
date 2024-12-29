/******************************************************************************
 *
 * @file       tcpmanager.h
 * @brief      client与chatServer的长连接
 *
 * @author     KBchulan
 * @date       2024/11/30
 * @history
 *****************************************************************************/
#ifndef TCPMANAGER_H
#define TCPMANAGER_H

#include "global.h"
#include "singleton.h"

#include <QTcpSocket>

class TcpManager final: public QObject, public Singleton<TcpManager>, public std::enable_shared_from_this<TcpManager>
{
    Q_OBJECT
    friend class Singleton<TcpManager>;

public:
    ~TcpManager();

private:
    TcpManager();

    void initHandlers();

    void handleMessage(ReqId id, int len, QByteArray body);

private:
    QTcpSocket _socket;
    QString _host;
    std::uint16_t _port;

    qint16 _message_id;
    qint16 _message_lenghth;
    QByteArray _buffer;

    // 头部
    bool _b_recv_pending;

    QMap<ReqId, std::function<void(ReqId id, int len, QByteArray body)>> _handlers;

public slots:
    void slot_tcp_connect(ServerInfo);

    void slot_send_data(ReqId id, QString body);

signals:
    void sig_con_success(bool success);

    void sig_login_failed(int);

    void sig_switch_chatdialog();

    void sig_send_data(ReqId id, QString body);

    void sig_get_orders_success();
};

#endif // TCPMANAGER_H
