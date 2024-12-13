#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include "const.h"

class LogicSystem;
class HttpConnection final : public std::enable_shared_from_this<HttpConnection>
{
    friend class LogicSystem;

public:
    explicit HttpConnection(boost::asio::io_context &ioc);

    // 开始监听
    void Start();

    // 获取socket
    boost::asio::ip::tcp::socket &GetSocket();

private:
    // 检测超时
    void CheckDeadline();

    // 受到数据后产生的应答函数
    void WriteResponse();

    // 处理请求(header, body)
    void HandleReq();

    // 解析get请求的参数
    void PreParseGetParam();

private:
    boost::asio::ip::tcp::socket _socket;
    boost::beast::flat_buffer _buffer{ BUFFER_SIZE };
    boost::beast::http::request<boost::beast::http::dynamic_body> _request;
    boost::beast::http::response<boost::beast::http::dynamic_body> _response;

    // 所有的定时器都是绑定在底层的事件循环里，需要绑定一个调度器
    boost::asio::steady_timer _deadline
    {
        _socket.get_executor(),
        std::chrono::seconds(60)
    };

    // get请求：url处理
    std::string _get_url;
    std::unordered_map<std::string, std::string> _get_params;
};

#endif // !HTTPCONNECTION_H
