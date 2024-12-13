#include "../include/CServer.h"
#include "../include/CSession.h"
#include "../include/LogManager.h"
#include "../include/AsioIOContextPool.h"

CServer::CServer(boost::asio::io_context& ioc, unsigned short port)
    : _ioc(ioc), _port(port), _acceptor(_ioc, boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::any(), _port))
{
    LOG_SERVER->info("CServer start success, on port: {}", _port);
    StartAccept();
}

CServer::~CServer()
{
    std::cout << "CServer has been destructed!\n";
}

void CServer::RemoveCSession(std::string Uuid)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _sessions.erase(Uuid);
}

void CServer::StartAccept()
{
    auto &ioc = AsioIOContextPool::GetInstance().GetIOContext();
    std::shared_ptr<CSession> new_session = std::make_shared<CSession>(ioc, this);

    _acceptor.async_accept(new_session->GetSocket(),
        [this, new_session](const boost::system::error_code &error)
        {
            HandleAccept(new_session, error);
        });
}

void CServer::HandleAccept(std::shared_ptr<CSession> session, const boost::system::error_code &ec)
{
    if(!ec)
    {
        session->Start();

        std::lock_guard<std::mutex> lock(_mutex);
        _sessions[session->GetUuid()] = session;
    }
    else
    {
        LOG_SERVER->error("Session accept failed, error is: {}", ec.message());
    }
    StartAccept();
}