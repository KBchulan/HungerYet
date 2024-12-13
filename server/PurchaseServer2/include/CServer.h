#ifndef CSERVER_H
#define CSERVER_H

#include <mutex>
#include <memory>
#include <iostream>
#include <unordered_map>
#include <boost/asio.hpp>

class CSession;
class CServer
{
public:
    explicit CServer(boost::asio::io_context& ioc, unsigned short port);

    ~CServer();

    void RemoveCSession(std::string Uuid);

private:
    void StartAccept();

    void HandleAccept(std::shared_ptr<CSession> session, const boost::system::error_code &ec);

private:
    std::mutex _mutex;
    unsigned short _port;
    boost::asio::io_context &_ioc;
    boost::asio::ip::tcp::acceptor _acceptor;

    std::unordered_map<std::string, std::shared_ptr<CSession>> _sessions;
};

#endif // !CSERVER_H