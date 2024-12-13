#include "../include/CServer.h"
#include "../include/LogManager.h"
#include "../include/ConfigManager.h"
#include "../include/AsioIOContextPool.h"

int main()
{
    if(!LogManager::GetInstance()->Init("../../../logs", LogLevel::DEBUG))
    {
        std::cerr << "初始化日志系统失败" << std::endl;
        return EXIT_FAILURE;
    }

    LOG_SERVER->info(R"(ChatServer start!)");

    try
    {
        auto &pool = AsioIOContextPool::GetInstance();

        boost::asio::io_context ioc;
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);

        signals.async_wait([&pool, &ioc](auto, auto) -> void
        {
            pool.Stop();
            ioc.stop();

            if(!LogManager::GetInstance()->IsShutdown())
            {
                LOG_SERVER->info("Shutting down server");
                LOG_SERVER->flush();
                LogManager::Shutdown();
            }
        });

        // create server
        std::shared_ptr<CServer> server = std::make_shared<CServer>(ioc, static_cast<unsigned short>(atoi(ConfigManager::GetInstance()["SelfServer"]["port"].c_str())));

        ioc.run();
    }
    catch (const std::exception &e)
    {
        if (!LogManager::GetInstance()->IsShutdown())
        {
            LOG_SERVER->info("Shutting down server");
            LOG_SERVER->flush();
            LogManager::Shutdown();
        }
        return EXIT_FAILURE;
    }
}