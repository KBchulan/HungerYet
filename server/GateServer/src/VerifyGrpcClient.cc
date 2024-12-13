#include "../include/LogManager.h"
#include "../include/ConfigManager.h"
#include "../include/VerifyGrpcClient.h"

RPConPool::RPConPool(std::size_t poolsize, std::string host, std::string port)
    : _poolSize(poolsize), _host(host), _port(port), _b_stop(false)
{
    for (std::size_t i = 0; i < _poolSize; i++)
    {
        std::shared_ptr<Channel> channel = grpc::CreateChannel(_host + ":" + _port,
            grpc::InsecureChannelCredentials());

        LOG_RPC->info("create channel to verify server, host is {}, port is {}", _host, _port);
        _connections.push(VarifyService::NewStub(channel));
    }
}

RPConPool::~RPConPool()
{
    std::lock_guard<std::mutex> lock(_mutex);
    Close();
    while(!_connections.empty())
        _connections.pop();
}

void RPConPool::Close()
{
    _b_stop = true;
    _cond.notify_all();
}

std::unique_ptr<VarifyService::Stub> RPConPool::GetConnection()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _cond.wait(lock, [this]()
    {
        if(_b_stop)
            return true;
        return !_connections.empty();
    });

    if(_b_stop)
        return nullptr;
    
    auto context = std::move(_connections.front());
    _connections.pop();
    return context;
}

void RPConPool::returnConnection(std::unique_ptr<VarifyService::Stub> context)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if(_b_stop)
        return;
    
    _connections.push(std::move(context));
    _cond.notify_one();
}

GetVarifyRsp VerifyGrpcClient::GetVarifyCode(std::string email)
{
    ClientContext context;
    GetVarifyReq request;
    GetVarifyRsp reply;
    
    request.set_email(email);

    auto stub = _pool->GetConnection();
    Status status = stub->GetVarifyCode(&context, request, &reply);
    
    LOG_RPC->info("get varify code, status is {}", status.ok());
    if (status.ok())
    {
        _pool->returnConnection(std::move(stub));
        return reply;
    }
    else
    {
        _pool->returnConnection(std::move(stub));
        reply.set_error(ErrorCodes::RPCFailed);
        return reply;
    }
}

VerifyGrpcClient::VerifyGrpcClient()
{
    auto &gCfgMgr = ConfigManager::GetInstance();
    std::string host = gCfgMgr["VerifyServer"]["host"];
    std::string port = gCfgMgr["VerifyServer"]["port"];
    _pool.reset(new RPConPool(std::thread::hardware_concurrency(), host, port));
}