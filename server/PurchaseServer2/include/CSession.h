#ifndef CSESSION_H
#define CSESSION_H

#include "MsgNode.h"

#include <queue>
#include <mutex>
#include <atomic>
#include <memory>
#include <boost/uuid/uuid_io.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/uuid/uuid_generators.hpp>

class CServer;
class CSession : public std::enable_shared_from_this<CSession>
{
public:
    explicit CSession(boost::asio::io_context &ioc, CServer *server);

    ~CSession();

    boost::asio::ip::tcp::socket &GetSocket();

    std::string &GetUuid();

    // 会话逻辑的启动，此处使用协程
    void Start();

    // 关闭当前会话
    void Close();

    // 发送char*类型数据
    void Send(const char *msg, short max_length, short msg_id);

    // 发送string类型数据
    void Send(std::string msg, short msg_id);

    // 使用协程发送数据
    void Send(std::shared_ptr<SendNode> msg_node);

private:
    // 发送回调
    void HandleWrite(const boost::system::error_code& error, std::shared_ptr<CSession> self);

    // 协程函数
    boost::asio::awaitable<void> HandleWrite();

    // 网络相关
    CServer *_server;
    boost::asio::io_context& _ioc;
    boost::asio::ip::tcp::socket _socket;

    // 自身的属性
    bool _close;
    std::string _uuid;

    // 接收
    std::shared_ptr<MsgNode> _recv_head_node;
    std::shared_ptr<RecvNode> _recv_msg_node;

    // 发送
    std::mutex _send_lock;
    std::atomic<bool> _is_sending;
    std::queue<std::shared_ptr<SendNode>> _send_queue;
};

class LogicNode
{
public:
    explicit LogicNode(std::shared_ptr<CSession>, std::shared_ptr<RecvNode>);

    std::shared_ptr<CSession> _session;
    std::shared_ptr<RecvNode> _recvnode;
};

#endif // !CSESSION_H