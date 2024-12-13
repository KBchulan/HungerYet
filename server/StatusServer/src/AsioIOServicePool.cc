#include "../include/LogManager.h"
#include "../include/AsioIOServicePool.h"

AsioIOServicePool::~AsioIOServicePool()
{
    // 经典RAII
    Stop();
    LOG_SERVER->info(R"(AsioIOServicePool has been destructed!)");
}

AsioIOServicePool::AsioIOServicePool(std::size_t size)
    : _nextIoContext(0), _ioContexts(size), _works(size)
{
    for (size_t i = 0; i < size; i++)
    {
        _works[i] = std::unique_ptr<boost::asio::io_context::work>(
            new boost::asio::io_context::work(_ioContexts[i]));
    }

    for (size_t i = 0; i < _ioContexts.size(); i++)
    {
        _threads.emplace_back([this, i]()
                              { _ioContexts[i].run(); });
    }
}

boost::asio::io_context &AsioIOServicePool::GetIoContext()
{
    std::size_t current = _nextIoContext.fetch_add(1, std::memory_order_relaxed) % _ioContexts.size();
    
    std::size_t expected = current + _ioContexts.size();
    if (expected > (std::numeric_limits<std::size_t>::max() / 2))
        _nextIoContext.store(0, std::memory_order_relaxed);
    
    return _ioContexts[current];
}

void AsioIOServicePool::Stop()
{
    for (auto &work : _works)
    {
        work->get_io_context().stop();
        work.reset();    
    }

    for (auto &thread : _threads)
        thread.join();
}