#ifndef MSGNODE_HPP
#define MSGNODE_HPP

#include "const.h"

#include <string>
#include <iostream>
#include <boost/asio.hpp>

class MsgNode
{
public:
    explicit MsgNode(short max_len) : _cur_len(0), _total_len(max_len)
    {
        _data = new char[_total_len + 1]();
        _data[_total_len] = '\0';
    }

    virtual ~MsgNode()
    {
        std::cerr << R"(MsgNode destruct!)" << '\n';
        delete[] _data;
    }

    void Clear()
    {
        memset(_data, 0, _total_len);
        _cur_len = 0;
    }

public:
    char *_data;      // 消息（TLV）
    short _cur_len;   // 当前处理的长度
    short _total_len; // 包括包头和包体
};

// 接收节点有包头有包体
class RecvNode final : public MsgNode
{
public:
    explicit RecvNode(short max_len, short msg_id) : MsgNode(max_len), _msg_id(msg_id)
    {
    }

    short GetMsgId() const
    {
        return _msg_id;
    }

private:
    short _msg_id;
};

// 构造发送节点传入的长度只有包体
class SendNode final : public MsgNode
{
public:
    explicit SendNode(const char *msg, short max_len, short msg_id) : MsgNode(max_len + HEAD_TOTAL_LEN), _msg_id(msg_id)
    {
        // 先发送ID
        short msg_id_net = boost::asio::detail::socket_ops::host_to_network_short(msg_id);
        memcpy(_data, &msg_id_net, HEAD_ID_LEN);

        // 数据长度
        short max_len_net = boost::asio::detail::socket_ops::host_to_network_short(max_len);
        memcpy(_data + HEAD_ID_LEN, &max_len_net, HEAD_DATA_LEN);

        // 包体
        memcpy(_data + HEAD_TOTAL_LEN, msg, max_len);
    }

    short GetMsgId() const
    {
        return _msg_id;
    }

private:
    short _msg_id;
};

#endif // !MSGNODE_HPP