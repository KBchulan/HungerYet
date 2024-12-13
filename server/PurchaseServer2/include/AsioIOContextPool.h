#ifndef ASIOIOCONTEXTPOOL_H
#define ASIOIOCONTEXTPOOL_H

#include <atomic>
#include <vector>
#include <memory>
#include <boost/asio.hpp>

class AsioIOContextPool
{
public:
    ~AsioIOContextPool();

    AsioIOContextPool(const AsioIOContextPool &) = delete;
    AsioIOContextPool &operator=(const AsioIOContextPool &) = delete;

    void Stop();

    boost::asio::io_context &GetIOContext();

    static AsioIOContextPool &GetInstance();

private:
    AsioIOContextPool(std::uint32_t size = std::thread::hardware_concurrency());

private:
    std::vector<std::thread> _threads;
    std::atomic<std::uint32_t> _nextIOContext;
    std::vector<boost::asio::io_context> _ioContexts;
    std::vector<std::unique_ptr<boost::asio::io_context::work>> _works;

};

#endif // !ASIOIOCONTEXTPOOL_H