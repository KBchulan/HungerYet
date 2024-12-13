#include "../include/AsioIOContextPool.h"

#include <iostream>

AsioIOContextPool::AsioIOContextPool(std::uint32_t size)
    : _nextIOContext(0), _ioContexts(size), _works(size)
{
    for(std::uint32_t i = 0; i < size; i++)
    {
        _works[i] = std::unique_ptr<boost::asio::io_context::work>(
            new boost::asio::io_context::work(_ioContexts[i])
        );
    }

    for(std::uint32_t i = 0; i < size; i++)
    {
        _threads.emplace_back([this, i]() -> void
        {
            _ioContexts[i].run();
        });
    }
}

AsioIOContextPool::~AsioIOContextPool()
{
    std::cout << "AsioIOContext has been destructed!" << '\n';
}

void AsioIOContextPool::Stop()
{
    for(auto &work : _works)
    {
        work->get_io_context().stop();
        work.reset();
    }
    
    for(auto &thread : _threads)
        thread.join();
}

boost::asio::io_context &AsioIOContextPool::GetIOContext()
{
    std::size_t current = _nextIOContext.fetch_add(1, std::memory_order_relaxed) % _ioContexts.size();
    
    std::size_t expected = current + _ioContexts.size();
    if (expected > (std::numeric_limits<std::size_t>::max() / 2))
        _nextIOContext.store(0, std::memory_order_relaxed);
    
    return _ioContexts[current];
}

AsioIOContextPool &AsioIOContextPool::GetInstance()
{
    static AsioIOContextPool instance;
    return instance;
}
