#include "../include/CServer.h"
#include "../include/CSession.h"
#include "../include/LogManager.h"
#include "../include/LogicSystem.h"

CSession::CSession(boost::asio::io_context &ioc, CServer *server) 
    : _ioc(ioc), _close(false), _server(server), _socket(_ioc), _is_sending(false)
{
    boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
    _uuid = boost::uuids::to_string(a_uuid);

    _recv_head_node = std::make_shared<MsgNode>(HEAD_TOTAL_LEN);
}

CSession::~CSession()
{
    try
    {
        std::cout << R"(CSession destruct!)" << '\n';
        Close();
    }
    catch (const std::exception &e)
    {
        std::cerr << R"(CSession destruct error, error is: )" << e.what() << '\n';
    }
}

boost::asio::ip::tcp::socket &CSession::GetSocket()
{
    return _socket;
}

std::string &CSession::GetUuid()
{
    return _uuid;
}

void CSession::Start()
{
    // 开启协程
    boost::asio::co_spawn(_ioc, [self = shared_from_this(), this]() -> boost::asio::awaitable<void>
    { 
        try
        {
            while(!_close)
            {
                _recv_head_node->Clear();

                // 先接收包头
                std::size_t n = co_await boost::asio::async_read(_socket, boost::asio::buffer(_recv_head_node->_data, HEAD_TOTAL_LEN), boost::asio::use_awaitable);

                if(n == 0)
                {
                    LOG_SERVER->info("Recvive peer closed!");
                    Close();
                    _server->RemoveCSession(_uuid);
                    co_return;
                }

                // 再接收包体

                // id
                short msg_id = 0;
                memcpy(&msg_id, _recv_head_node->_data, HEAD_ID_LEN);
                msg_id = boost::asio::detail::socket_ops::network_to_host_short(msg_id);

                LOG_SERVER->info("Msg id is: {}", msg_id);
                if(msg_id > MAX_LENGTH)
                {
                    LOG_SERVER->error("Invalid msg id is: {}", msg_id);
                    Close();
                    _server->RemoveCSession(_uuid);
                    co_return;
                }

                // len
                short msg_len = 0;
                memcpy(&msg_len, _recv_head_node->_data + HEAD_ID_LEN, HEAD_DATA_LEN);
                msg_len = boost::asio::detail::socket_ops::network_to_host_short(msg_len);

                std::cout << R"(Msg len is: )" << msg_len << '\n';
                if(msg_len > MAX_LENGTH)
                {
                    LOG_SERVER->error("Invalid msg len is: {}", msg_len);
                    Close();
                    _server->RemoveCSession(_uuid);
                    co_return;
                }

                _recv_msg_node = std::make_shared<RecvNode>(msg_len, msg_id);

                // body
                n = co_await boost::asio::async_read(_socket, boost::asio::buffer(_recv_msg_node->_data, _recv_msg_node->_total_len), boost::asio::use_awaitable);       

                if (n == 0)
                {
                    LOG_SERVER->info("Recvive peer closed!");
                    Close();
                    _server->RemoveCSession(_uuid);
                    co_return;
                }

                LOG_SERVER->info("Receive data is: {}", _recv_msg_node->_data);

                // 投递给逻辑线程处理
                LogicSystem::GetInstance().PostMsgToQue(std::make_shared<LogicNode>(shared_from_this(), _recv_msg_node));
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << R"(CSession start failed, exception is: )" << e.what() << '\n';
            Close();
            _server->RemoveCSession(_uuid);
        } 
    }, boost::asio::detached);
}

void CSession::Close()
{
    if(_close)
        return;

    _socket.close();
    _close = true;
}

void CSession::Send(const char *msg, short max_length, short msg_id)
{
    std::unique_lock<std::mutex> lock(_send_lock);
    int send_que_size = _send_queue.size();

    if (send_que_size > MAX_SENDQUE)
    {
        LOG_SERVER->error("Session {} Send queue fulled, size is: {}", _uuid, MAX_SENDQUE);
        return;
    }

    _send_queue.push(std::make_shared<SendNode>(msg, max_length, msg_id));

    if (send_que_size > 0)
        return;

    auto msg_node = _send_queue.front();
    lock.unlock();

    boost::asio::async_write(_socket, boost::asio::buffer(msg_node->_data, msg_node->_total_len),
    [this, self = shared_from_this()](const boost::system::error_code &error, auto)
    {
        HandleWrite(error, self);
    });
}

void CSession::Send(std::string msg, short msg_id)
{
    Send(msg.c_str(), msg.length(), msg_id);
}

void CSession::Send(std::shared_ptr<SendNode> msg_node)
{
    bool need_send = false;
    {
        std::lock_guard<std::mutex> lock(_send_lock);
        _is_sending = false;
        _send_queue.push(msg_node);
    }

    if (need_send && !_is_sending.exchange(true))
    {
        boost::asio::co_spawn(_ioc, [self = shared_from_this()]() -> boost::asio::awaitable<void>
        {
            try
            {
                co_await self->HandleWrite();
            }
            catch (const std::exception &e)
            {
                LOG_SERVER->error("Send coroutine error: {}", e.what());
                self->Close();
            } 
        }, boost::asio::detached);
    }
}

void CSession::HandleWrite(const boost::system::error_code &error, std::shared_ptr<CSession> self)
{
    try
    {
        if (!error)
        {
            std::unique_lock<std::mutex> lock(_send_lock);
            _send_queue.pop();

            if (!_send_queue.empty())
            {
                auto msg_node = _send_queue.front();
                lock.unlock();

                boost::asio::async_write(_socket, boost::asio::buffer(msg_node->_data, msg_node->_total_len),
                [this, self = shared_from_this()](const boost::system::error_code &error, auto)
                {
                    HandleWrite(error, self);
                });
            }
        }
        else
        {
            LOG_SERVER->error("HandleWrite failed, error is: {}", error.message());
            Close();
            _server->RemoveCSession(_uuid);
        }
    }
    catch (const std::exception &e)
    {
        LOG_SERVER->error("HandleWrite exception is: {}", e.what());
        Close();
        _server->RemoveCSession(_uuid);
    }
}

boost::asio::awaitable<void> CSession::HandleWrite()
{
    while (!_close)
    {
        std::shared_ptr<SendNode> current_msg;
        {
            std::lock_guard<std::mutex> lock(_send_lock);
            if (_send_queue.empty())
            {
                _is_sending = false;
                co_return;
            }
            current_msg = _send_queue.front();
            _send_queue.pop();
        }

        try
        {
            // 发送头部
            short msg_id = boost::asio::detail::socket_ops::host_to_network_short(current_msg->GetMsgId());
            short msg_len = boost::asio::detail::socket_ops::host_to_network_short(current_msg->_total_len);

            char head[HEAD_TOTAL_LEN] = {0};
            memcpy(head, &msg_id, HEAD_ID_LEN);
            memcpy(head + HEAD_ID_LEN, &msg_len, HEAD_DATA_LEN);

            co_await boost::asio::async_write(
                _socket,
                boost::asio::buffer(head, HEAD_TOTAL_LEN),
                boost::asio::use_awaitable);

            // 发送数据体
            co_await boost::asio::async_write(
                _socket,
                boost::asio::buffer(current_msg->_data, current_msg->_total_len),
                boost::asio::use_awaitable);

            LOG_SERVER->info("Send message successfully, id: {}, len: {}", current_msg->GetMsgId(), current_msg->_total_len);
        }
        catch (const std::exception &e)
        {
            LOG_SERVER->error("Send error: {}", e.what());
            _is_sending = false;
            throw;
        }
    }

    _is_sending = false;
}

LogicNode::LogicNode(std::shared_ptr<CSession> session, std::shared_ptr<RecvNode> recvnode)
    : _session(session), _recvnode(recvnode)
{
}