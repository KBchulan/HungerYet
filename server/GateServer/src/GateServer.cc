#include "../include/CServer.h"
#include "../include/LogManager.h"
#include "../include/ConfigManager.h"

int main()
{
    if(!LogManager::GetInstance()->Init("../../../logs", LogLevel::DEBUG))
    {
        std::cerr << "初始化日志系统失败" << std::endl;
        return EXIT_FAILURE;
    }

    LOG_SERVER->info(R"(GateServer start!)");

    try
    {
        unsigned short port = static_cast<unsigned short>(atoi(ConfigManager::GetInstance()["GateServer"]["port"].c_str()));
        boost::asio::io_context ioc;

        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);

        signals.async_wait([&ioc](boost::system::error_code error, auto)
        {
            if (error)
            {
                LOG_SERVER->error(R"({} : {})", __FILE__, "Singal receive failed!");
                return 0;
            }

            if(!LogManager::GetInstance()->IsShutdown())
            {
                LOG_SERVER->info("Shutting down server");
                LOG_SERVER->flush();
                LogManager::Shutdown();
            }

            ioc.stop();
            return 0;
        });

        std::make_shared<CServer>(ioc, port)->Start();
        ioc.run();
    }
    catch (const std::exception &e)
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