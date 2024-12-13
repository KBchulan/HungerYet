#include "../include/LogManager.h"
#include "../include/RedisManager.h"
#include "../include/ConfigManager.h"

RedisConnectPool::RedisConnectPool(std::uint32_t size, std::string host, std::uint16_t port, const char *password)
    : _poolSize(size), _host(host), _port(port), _b_stop(false)
{
    for(size_t i = 0; i < _poolSize; i++)
    {
        auto *context = redisConnect(_host.c_str(), _port);

        // connect failed
        if(context == nullptr || context->err != 0)
        {
            if(context != nullptr)
                redisFree(context);
            continue;
        }

        auto reply = (redisReply *)redisCommand(context, "AUTH %s", password);

        // auth failed
        if(reply == nullptr)
        {
            std::cerr << "认证失败" << '\n';
            redisFree(context);
            continue;
        }
        if(reply->type == REDIS_REPLY_ERROR)
        {
            std::cerr << "认证失败" << '\n';
            redisFree(context);
            freeReplyObject(reply); 
            continue;
        }

        freeReplyObject(reply);
        _connections.push(std::move(context));
    }
}

RedisConnectPool::~RedisConnectPool()
{
    std::lock_guard<std::mutex> lock(_mutex);
    while(!_connections.empty())
    {
        auto* conn = _connections.front();
        redisFree(conn);
        _connections.pop();
    }
}

redisContext *RedisConnectPool::GetRedisConnection()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _cond.wait(lock, [this]()
    {
        if(_b_stop)
            return true;
        return !_connections.empty();
    });

    auto *context = std::move(_connections.front());
    _connections.pop();
    return context;
}

void RedisConnectPool::returnRedisConnection(redisContext *context)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if(_b_stop)
        return;
    
    _connections.push(std::move(context));
    _cond.notify_one();
}

void RedisConnectPool::Close()
{
    _b_stop = true;
    _cond.notify_all();
}

RedisManager::RedisManager()
{
    auto gCfgMgr = ConfigManager::GetInstance();
    std::string host = gCfgMgr["Redis"]["host"];
    std::string port = gCfgMgr["Redis"]["port"];
    std::string password = gCfgMgr["Redis"]["password"];
    _con_pool.reset(new RedisConnectPool(std::thread::hardware_concurrency(), host, static_cast<std::uint16_t>(atoi(port.c_str())), password.c_str()));
}

RedisManager::~RedisManager()
{
    Close();
}

bool RedisManager::Get(const std::string &key, std::string &value)
{
    auto connect = _con_pool->GetRedisConnection();
    if(connect == nullptr)
        return false;
    
    ConnectionGuard guard(_con_pool.get(), connect);

    auto reply = (redisReply *)redisCommand(connect, "GET %s", key.c_str());

    if(reply == nullptr)
    {
        LOG_Redis->error("Execute command, [ Get {} ] falied!", key);
        return false;
    }

    if(reply->type != REDIS_REPLY_STRING)
    {
        LOG_Redis->error("Execute command, [ Get {} ] falied!", key);
        freeReplyObject(reply);
        return false;
    }

    value = reply->str;
    freeReplyObject(reply);
    LOG_Redis->info("Execute command, [ Get {} ] success!", key);
    return true; 
}

bool RedisManager::Set(const std::string &key, const std::string &value)
{
    auto connect = _con_pool->GetRedisConnection();
    if(connect == nullptr)
        return false;
    
    ConnectionGuard guard(_con_pool.get(), connect);

   auto reply = (redisReply *)redisCommand(connect, "SET %s %s", key.c_str(), value.c_str());

   if(reply == nullptr)
    {
        LOG_Redis->error("Execute command [ SET {} {} ] failed!", key, value);
        return false; 
    }

    if(!(reply->type == REDIS_REPLY_STATUS && strcasecmp(reply->str, "OK") == 0))
    {
        LOG_Redis->error("Execute command [ SET {} {} ] failed!", key, value);
        freeReplyObject(reply);
        return false; 
    }

    freeReplyObject(reply);
    LOG_Redis->info("Execute command [ SET {} {} ] success!", key, value);
    return true;
}

bool RedisManager::Auth(const std::string &password)
{
    auto connect = _con_pool->GetRedisConnection();
    if(connect == nullptr)
        return false;

    ConnectionGuard guard(_con_pool.get(), connect);
    
    auto reply = (redisReply *)redisCommand(connect, "AUTH %s", password.c_str());


    if(reply == nullptr)
    {
        LOG_Redis->error("Password error");
        return false;
    }

    if(reply->type == REDIS_REPLY_ERROR)
    {
        LOG_Redis->error("Password error");
        freeReplyObject(reply);
        return false;
    }

    freeReplyObject(reply);
    LOG_Redis->info("Password right");
    return true;
}

bool RedisManager::LPush(const std::string &key, const std::string &value)
{
    auto connect = _con_pool->GetRedisConnection();
    if(connect == nullptr)
        return false;

    ConnectionGuard guard(_con_pool.get(), connect);
    
    auto reply = (redisReply *)redisCommand(connect, "LPUSH %s %s", key.c_str(), value.c_str());

    if(reply == nullptr)
    {
        LOG_Redis->error("Execute command [ LPUSH {} {} ] failed!", key, value);
        return false;
    }

    if(reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0)
    {
        LOG_Redis->error("Execute command [ LPUSH {} {} ] failed!", key, value);
        freeReplyObject(reply);
        return false;
    }

    freeReplyObject(reply);
    LOG_Redis->info("Execute command [ LPUSH {} {} ] success!", key, value);
    return true;
}

bool RedisManager::LPop(const std::string &key, std::string &value)
{
    auto connect = _con_pool->GetRedisConnection();
    if(connect == nullptr)
        return false;

    ConnectionGuard guard(_con_pool.get(), connect);
    
    auto reply = (redisReply *)redisCommand(connect, "LPOP %s", key.c_str());

    if(reply == nullptr)
    {
        LOG_Redis->error("Execute command [ LPOP {} ] failed!", key);
        return false;
    }

    if(reply->type != REDIS_REPLY_STRING)
    {
        LOG_Redis->error("Execute command [ LPOP {} ] failed!", key);
        freeReplyObject(reply);
        return false;
    }

    value = reply->str;
    freeReplyObject(reply);
    LOG_Redis->info("Execute command [ LPOP {} ] success!", key);
    return true; 
}

bool RedisManager::RPush(const std::string &key, const std::string &value)
{
    auto connect = _con_pool->GetRedisConnection();
    if(connect == nullptr)
        return false;

    ConnectionGuard guard(_con_pool.get(), connect);
    
    auto reply = (redisReply *)redisCommand(connect, "RPUSH %s %s", key.c_str(), value.c_str());

    if(reply == nullptr)
    {
        LOG_Redis->error("Execute command [ RPUSH {} {} ] failed!", key, value);
        return false;
    }

    if(reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0)
    {
        LOG_Redis->error("Execute command [ RPUSH {} {} ] failed!", key, value);
        freeReplyObject(reply);
        return false;
    }

    freeReplyObject(reply);
    LOG_Redis->info("Execute command [ RPUSH {} {} ] success!", key, value);
    return true;
}

bool RedisManager::RPop(const std::string &key, std::string &value)
{
    auto connect = _con_pool->GetRedisConnection();
    if(connect == nullptr)
        return false;

    ConnectionGuard guard(_con_pool.get(), connect);
    
    auto reply = (redisReply *)redisCommand(connect, "RPOP %s", key.c_str());

    if(reply == nullptr)
    {
        LOG_Redis->error("Execute command [ RPOP {} ] failed!", key);
        return false;
    }

    if(reply->type != REDIS_REPLY_STRING)
    {
        LOG_Redis->error("Execute command [ RPOP {} ] failed!", key);
        freeReplyObject(reply);
        return false;
    }

    value = reply->str;
    freeReplyObject(reply);
    LOG_Redis->info("Execute command [ RPOP {} ] success!", key);
    return true;
}

bool RedisManager::HSet(const std::string &key, const std::string &hkey, const std::string &value)
{
    auto connect = _con_pool->GetRedisConnection();
    if(connect == nullptr)
        return false;

    ConnectionGuard guard(_con_pool.get(), connect);
    
    auto reply = (redisReply *)redisCommand(connect, "HSET %s %s %s", key.c_str(), hkey.c_str(), value.c_str());

    if(reply == nullptr)
    {
        LOG_Redis->error("Execute command [ HSET {} {} {} ] failed!", key, hkey, value);
        return false;
    }

    if(reply->type != REDIS_REPLY_INTEGER)
    {
        LOG_Redis->error("Execute command [ HSET {} {} {} ] failed!", key, hkey, value);
        freeReplyObject(reply);
        return false;
    }

    freeReplyObject(reply);
    LOG_Redis->info("Execute command [ HSET {} {} {} ] success!", key, hkey, value);
    return true;
}

bool RedisManager::HSet(const char *key, const char *hkey, const char *hvalue, size_t hvaluelen)
{
    const char *argv[4];
    size_t argvlen[4];
    argv[0] = "HSET";
    argvlen[0] = 4;
    argv[1] = key;
    argvlen[1] = strlen(key);
    argv[2] = hkey;
    argvlen[2] = strlen(hkey);
    argv[3] = hvalue;
    argvlen[3] = hvaluelen;

    auto connect = _con_pool->GetRedisConnection();
    if(connect == nullptr)
        return false;

    ConnectionGuard guard(_con_pool.get(), connect);
    
    auto reply = (redisReply *)redisCommandArgv(connect, 4, argv, argvlen);

    if (reply == nullptr)
    {
        LOG_Redis->error("Execut command [ HSet {} {} {} ] failure ! ", key, hkey, hvalue);
        return false;
    }

    if (reply->type != REDIS_REPLY_INTEGER)
    {
        LOG_Redis->error("Execut command [ HSet {} {} {} ] failure ! ", key, hkey, hvalue);
        freeReplyObject(reply);
        return false;
    }

    LOG_Redis->info("Execut command [ HSet {} {} {} ] success ! ", key, hkey, hvalue);
    freeReplyObject(reply);
    return true;
}

std::string RedisManager::HGet(const std::string &key, const std::string &hkey)
{
    const char *argv[3];
    size_t argvlen[3];
    argv[0] = "HGET";
    argvlen[0] = 4;
    argv[1] = key.c_str();
    argvlen[1] = key.length();
    argv[2] = hkey.c_str();
    argvlen[2] = hkey.length();

    auto connect = _con_pool->GetRedisConnection();
    if(connect == nullptr)
        return "";

    ConnectionGuard guard(_con_pool.get(), connect);
    
    auto reply = (redisReply *)redisCommandArgv(connect, 3, argv, argvlen);

    if (reply == nullptr)
    {
        LOG_Redis->error("Execut command [ HGet {} {} ] failure ! ", key, hkey);
        return "";
    }

    if (reply->type == REDIS_REPLY_NIL)
    {
        freeReplyObject(reply);
        LOG_Redis->error("Execut command [ HGet {} {} ] failure ! ", key, hkey);
        return "";
    }

    std::string value = reply->str;
    freeReplyObject(reply);
    LOG_Redis->info("Execut command [ HGet {} {} ] success ! ", key, hkey);
    return value;
}

bool RedisManager::Del(const std::string &key)
{
    auto connect = _con_pool->GetRedisConnection();
    if(connect == nullptr)
        return false;

    ConnectionGuard guard(_con_pool.get(), connect);
    
    auto reply = (redisReply *)redisCommand(connect, "DEL %s", key.c_str());

    if (reply == nullptr)
    {
        LOG_Redis->error("Execut command [ Del {} ] failure ! ", key);
        return false;
    }

    if (reply->type != REDIS_REPLY_INTEGER)
    {
        LOG_Redis->error("Execut command [ Del {} ] failure ! ", key);
        freeReplyObject(reply);
        return false;
    }

    LOG_Redis->info("Execut command [ Del {} ] success ! ", key);
    freeReplyObject(reply);
    return true;
}

bool RedisManager::ExistsKey(const std::string &key)
{
    auto connect = _con_pool->GetRedisConnection();
    if(connect == nullptr)
        return false;

    ConnectionGuard guard(_con_pool.get(), connect);
    
    auto reply = (redisReply *)redisCommand(connect, "exists %s", key.c_str());

    if(reply == nullptr)
    {
        LOG_Redis->error("Not Found [ Key {} ]  ! ", key);
        return false;
    }

    if(reply->type != REDIS_REPLY_INTEGER || reply->integer == 0)
    {
        LOG_Redis->error("Not Found [ Key {} ]  ! ", key);
        freeReplyObject(reply);
        return false;
    }
    
    LOG_Redis->info("Found [ Key {} ] exists ! ", key);
    freeReplyObject(reply);
    return true;
}

void RedisManager::Close()
{
    _con_pool->Close();
}
