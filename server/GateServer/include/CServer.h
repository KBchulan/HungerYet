#ifndef CSERVER_H
#define CSERVER_H

#include "const.h"

class CServer final : public std::enable_shared_from_this<CServer>
{
public:
    CServer(boost::asio::io_context &ioc, unsigned short port);

    void Start();

private:
    boost::asio::io_context &_ioc;
    boost::asio::ip::tcp::acceptor _acceptor;
};

#endif // !CSERVER_H