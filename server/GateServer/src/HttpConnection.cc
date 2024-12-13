#include "../include/LogManager.h"
#include "../include/LogicSystem.h"
#include "../include/HttpConnection.h"

#include <iostream>

HttpConnection::HttpConnection(boost::asio::io_context &ioc)
    : _socket(ioc)

{
    LOG_HTTP->info(R"(HttpManager has been created!)");
}

void HttpConnection::Start()
{
    auto self = shared_from_this();
    boost::beast::http::async_read(_socket, _buffer, _request, [self](boost::beast::error_code ec, std::size_t bytes_transferred)
    {
        try
        {
            if(ec)
            {
                LOG_HTTP->error(R"({} : {})", __FILE__, ec.message());
                return;
            }

            boost::ignore_unused(bytes_transferred);
            self->HandleReq();
            self->CheckDeadline();
        }
        catch(const std::exception& e)
        {
            LOG_HTTP->error(R"({} : {})", __FILE__, e.what());
        }
    });
}

void HttpConnection::CheckDeadline()
{
    auto self = shared_from_this();
    _deadline.async_wait([self](boost::beast::error_code ec)
    {
        if(!ec)
        {
            self->_socket.close(ec);
        }
    });
}

void HttpConnection::WriteResponse()
{
    auto self = shared_from_this();
    _response.content_length(_response.body().size());

    boost::beast::http::async_write(_socket, _response, [self](boost::beast::error_code ec, std::size_t bytes_transferred)
    {
        self->_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
        self->_deadline.cancel();
    });
}

unsigned char ToHex(unsigned char dec)
{
    std::stringstream ss;
    ss << std::hex << std::uppercase << static_cast<int>(dec);
    std::string hex = ss.str();

    if (hex.length() == 1)
        hex = "0" + hex;
    return static_cast<unsigned char>(std::stoi(hex, nullptr, 16));
}

unsigned char FromHex(unsigned char hex)
{
    if (hex >= '0' && hex <= '9')
        return hex - '0';
    if (hex >= 'A' && hex <= 'F')
        return hex - 'A' + 10;
    if (hex >= 'a' && hex <= 'f')
        return hex - 'a' + 10;
    return 0;
}

// url编码
std::string UrlEncode(const std::string &str)
{
    std::string result;
    for (char c : str)
    {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
            result += c;
        else if (c == ' ')
            result += '+';
        else
        {
            result += '%';
            result += ToHex(static_cast<unsigned char>(c) >> 4);
            result += ToHex(static_cast<unsigned char>(c) & 0x0F);
        }
    }
    return result;
}

// url解码
std::string UrlDecode(const std::string &str)
{
    std::string result;
    size_t len = str.length();
    for (size_t i = 0; i < len; i++)
    {
        if (str[i] == '+')
            result += ' ';
        else if (str[i] == '%' && i + 2 < len)
        {
            unsigned char high = FromHex(str[++i]);
            unsigned char low = FromHex(str[++i]);
            result += static_cast<char>((high << 4) + low);
        }
        else
            result += str[i];
    }
    return result;
}

void HttpConnection::HandleReq()
{
    // 设置版本
    _response.version(_request.version());
    _response.keep_alive(false);

    if(_request.method() == boost::beast::http::verb::get)
    {
        PreParseGetParam();
        bool success = LogicSystem::GetInstance()->HandleGet(_get_url, shared_from_this());
        if(!success)
        {
            _response.result(boost::beast::http::status::not_found);
            _response.set(boost::beast::http::field::content_type, "text/plain");
            boost::beast::ostream(_response.body()) << "url not found\r\n";
            WriteResponse();
            return;
        }

        _response.result(boost::beast::http::status::ok);
        _response.set(boost::beast::http::field::server, "GateServer");
        WriteResponse();
        return;
    }

    if(_request.method() == boost::beast::http::verb::post)
    {
        bool success = LogicSystem::GetInstance()->HandlePost(_request.target().to_string(), shared_from_this());
        if(!success)
        {
            _response.result(boost::beast::http::status::not_found);
            _response.set(boost::beast::http::field::content_type, "text/plain");
            boost::beast::ostream(_response.body()) << "url not found\r\n";
            WriteResponse();
            return;
        }

        _response.result(boost::beast::http::status::ok);
        _response.set(boost::beast::http::field::server, "GateServer");
        WriteResponse();
        return;
    }
}

void HttpConnection::PreParseGetParam()
{
    auto uri = _request.target().to_string();

    // 查找?的位置，没有就不用解析params
    auto query_pos = uri.find('?');
    if(query_pos == std::string::npos)
    {
        _get_url = uri;
        return;
    }
    _get_url = uri.substr(0, query_pos);

    // ？后面的键值对
    std::string query_string = uri.substr(query_pos + 1);
    std::string key;
    std::string value;
    size_t pos = 0;

    while ((pos = query_string.find('&')) != std::string::npos)
    {
        auto pair = query_string.substr(0, pos);
        size_t eq_pos = pair.find('=');

        if(eq_pos != std::string::npos)
        {
            key = UrlDecode(pair.substr(0, eq_pos));
            value = UrlDecode(pair.substr(eq_pos + 1));
            _get_params[key] = value;
        }
        query_string.erase(0, pos + 1);
    }
    // 处理最后一个键值对
    if(!query_string.empty())
    {
        size_t eq_pos = query_string.find('=');

        if(eq_pos != std::string::npos)
        {
            key = UrlDecode(query_string.substr(0, eq_pos));
            value = UrlDecode(query_string.substr(eq_pos + 1));
            _get_params[key] = value;
        }
    }
}

boost::asio::ip::tcp::socket &HttpConnection::GetSocket()
{
    return _socket;
}
