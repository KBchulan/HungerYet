#include "../include/LogManager.h"
#include "../include/ConfigManager.h"
#include "../include/StatusServiceImpl.h"

#include <grpcpp/grpcpp.h>

void RunServer()
{
    auto &cfg = ConfigManager::GetInstance();

    std::string server_address(cfg["StatusServer"]["host"] + ":" + cfg["StatusServer"]["port"]);
    StatusServiceImpl service;

    grpc::ServerBuilder builder;

    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    // grpc服务器
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    LOG_SERVER->info("StatusServer listen on: {}", server_address);

    boost::asio::io_context ioc;
    boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);

    signals.async_wait([&ioc, &server](const boost::system::error_code &error, auto)
    {
        if (error)
        {
            LOG_SERVER->error(R"({} : {})", __FILE__, "Singal receive failed!");
            return;
        }

        if(!LogManager::GetInstance()->IsShutdown())
        {
            LOG_SERVER->info("Shutting down server");
            LOG_SERVER->flush();
            LogManager::Shutdown();
        }
        server->Shutdown();
        ioc.stop();
    });

    std::thread([&ioc]{ ioc.run(); }).detach();

    server->Wait();
}

int main()
{
    if(!LogManager::GetInstance()->Init("../../../logs", LogLevel::DEBUG))
    {
        std::cerr << "初始化日志系统失败" << std::endl;
        return EXIT_FAILURE;
    }

    LOG_SERVER->info(R"(StatusServer start!)");

    try
    {
        RunServer();
    }
    catch(const std::exception& e)
    {
        if(!LogManager::GetInstance()->IsShutdown())
        {
            LOG_SERVER->info("Shutting down server");
            LOG_SERVER->flush();
            LogManager::Shutdown();
        }
        return EXIT_FAILURE;
    }
    
}