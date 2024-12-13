#ifndef ASIOIOSERVICEPOOL_H
#define ASIOIOSERVICEPOOL_H

#include "const.h"

class AsioIOServicePool final : public Singleton<AsioIOServicePool>
{
    friend class Singleton<AsioIOServicePool>;
    
public:
    ~AsioIOServicePool();

    void Stop();

    boost::asio::io_context &GetIoContext();

public:
    AsioIOServicePool(const AsioIOServicePool &) = delete;
    AsioIOServicePool &operator=(const AsioIOServicePool &) = delete;

private:
    AsioIOServicePool(std::size_t size = std::thread::hardware_concurrency());

private:
    std::vector<std::thread> _threads;
    std::atomic<std::size_t> _nextIoContext;
    std::vector<boost::asio::io_context> _ioContexts;
    std::vector<std::unique_ptr<boost::asio::io_context::work>> _works;
};

#endif // !ASIOIOSERVICEPOOL_H