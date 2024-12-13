#ifndef REDISMANAGER_H
#define REDISMANAGER_H

#include "const.h"
#include "Singleton.h"

#include <queue>
#include <atomic>
#include <hiredis/hiredis.h>
#include <condition_variable>

// 二段式验证可以避免段错误
class RedisConnectPool
{
public:
    RedisConnectPool(std::uint32_t size, std::string host, std::uint16_t port, const char *password);

    ~RedisConnectPool();

    redisContext *GetRedisConnection();

    void returnRedisConnection(redisContext *context);

    void Close();

private:
    std::mutex _mutex;
    std::string _host;
    std::uint16_t _port;
    std::uint32_t _poolSize;
    std::atomic_bool _b_stop;
    std::condition_variable _cond;
    std::queue<redisContext *> _connections;
};

class RedisManager final : public Singleton<RedisManager>
{
    friend class Singleton<RedisManager>;

public:
    ~RedisManager();

    // 这些都只是对redis语句的简单封装，增加了错误处理

    // 获取键值对中的值
    bool Get(const std::string &key, std::string &value);

    // 设置键值对
    bool Set(const std::string &key, const std::string &value);

    // Redis身份验证
    bool Auth(const std::string &password);

    // 从列表左端插入元素
    bool LPush(const std::string &key, const std::string &value);

    // 从列表左端弹出元素
    bool LPop(const std::string &key, std::string& value);

    // 从列表右端插入元素
    bool RPush(const std::string& key, const std::string& value);

    // 从列表右端弹出元素
    bool RPop(const std::string& key, std::string& value);

    // 设置哈希表字段的值
    bool HSet(const std::string &key, const std::string  &hkey, const std::string &value);

    // 设置哈希表字段的值(原始字符串版本)
    bool HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen);

    // 获取哈希表字段的值
    std::string HGet(const std::string &key, const std::string &hkey);

    // 删除键值对
    bool Del(const std::string &key);

    // 检查键是否存在
    bool ExistsKey(const std::string &key);
    
    // 关闭Redis连接
    void Close();

private:
    // RAII释放
    class ConnectionGuard
    {
    public:
        ConnectionGuard(RedisConnectPool *pool, redisContext *conn)
            : _pool(pool), _conn(conn) {}

        ~ConnectionGuard()
        {
            if (_pool && _conn)
                _pool->returnRedisConnection(_conn);
        }

        ConnectionGuard(const ConnectionGuard &) = delete;
        ConnectionGuard &operator=(const ConnectionGuard &) = delete;

    private:
        RedisConnectPool *_pool;
        redisContext *_conn;
    };

private:
    RedisManager();

private:
    std::unique_ptr<RedisConnectPool> _con_pool;
};

#endif // !REDISMANAGER_H