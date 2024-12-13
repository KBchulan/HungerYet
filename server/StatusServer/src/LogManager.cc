#include "../include/LogManager.h"

LogManager::~LogManager()
{
    if(!_is_shutdown)
    {
        try
        {
            Flush();
            spdlog::shutdown();
        }
        catch (const std::exception &exc)
        {
            std::cerr << "LogManager destruct error: " << exc.what() << '\n';
        }
    }
}

bool LogManager::Init(const std::string &log_path, LogLevel level)
{
    try
    {
        _log_path = log_path;

        // 创建日志目录
        if (!boost::filesystem::exists(_log_path))
        {
            boost::filesystem::create_directories(_log_path);
        }

        // 初始化异步日志
        spdlog::init_thread_pool(8192, 1);

        // 设置日志格式
        // [时间] [日志级别] [线程id] [文件:行号] 日志内容
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");

        // 创建不同类型的logger
        _server_logger = CreateLogger("server", "server.log");
        _http_logger = CreateLogger("http", "http.log");
        _rpc_logger = CreateLogger("rpc", "rpc.log");
        _sql_logger = CreateLogger("sql", "sql.log");
        _redis_logger = CreateLogger("redis", "redis.log");

        if (!_server_logger || !_http_logger || !_rpc_logger || !_sql_logger || !_redis_logger)
        {
            return false;
        }

        // 设置日志级别
        SetLevel(level);

        LOG_SERVER->info("LogManager initialized successfully");
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "LogManager init failed: " << e.what() << std::endl;
        return false;
    }
}

std::shared_ptr<spdlog::logger> LogManager::CreateLogger(
    const std::string &logger_name,
    const std::string &file_name)
{
    try
    {
        // 创建控制台sink和文件sink
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
            _log_path + "/" + file_name, 0, 0);

        std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};

        // 创建异步logger
        auto logger = std::make_shared<spdlog::async_logger>(
            logger_name,
            sinks.begin(),
            sinks.end(),
            spdlog::thread_pool(),
            spdlog::async_overflow_policy::block);

        // 注册logger
        spdlog::register_logger(logger);

        return logger;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Create logger failed: " << e.what() << std::endl;
        return nullptr;
    }
}

void LogManager::SetLevel(LogLevel level)
{
    spdlog::set_level(static_cast<spdlog::level::level_enum>(level));
}

void LogManager::Flush()
{
    spdlog::apply_all([](std::shared_ptr<spdlog::logger> logger)
                      { logger->flush(); });
}