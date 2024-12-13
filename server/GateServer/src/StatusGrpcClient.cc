#include "../include/LogManager.h"
#include "../include/StatusGrpcClient.h"

StatusConPool::StatusConPool(std::uint32_t size, std::string host, std::string port)
    : _poolSize(size), _host(host), _port(port), _b_stop(false)
{
    for (std::size_t i = 0; i < _poolSize; i++)
    {
        std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port,
            grpc::InsecureChannelCredentials());

        LOG_RPC->info("create channel to status server, host is {}, port is {}", host, port);
        _connections.push(StatusService::NewStub(channel));
    }
}

StatusConPool::~StatusConPool()
{
    std::lock_guard<std::mutex> lock(_mutex);
    Close();
    while(!_connections.empty())
        _connections.pop();
}

std::unique_ptr<StatusService::Stub> StatusConPool::GetConnection()
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
    
    auto connection = std::move(_connections.front());
    _connections.pop();
    return connection;
}

void StatusConPool::returnConnection(std::unique_ptr<StatusService::Stub> context)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if(_b_stop)
        return;
    
    _connections.push(std::move(context));
    _cond.notify_one();
}

void StatusConPool::Close()
{
    _b_stop = true;
    _cond.notify_all();
}

StatusGrpcClient::StatusGrpcClient()
{
    LOG_SERVER->info("Initializing StatusGrpcClient");

    auto &gCfgMgr = ConfigManager::GetInstance();
    std::string host = gCfgMgr["StatusServer"]["host"];
    std::string port = gCfgMgr["StatusServer"]["port"];

    _pool.reset(new StatusConPool(std::thread::hardware_concurrency(), host, port));
}

StatusGrpcClient::~StatusGrpcClient()
{
}

GetChatServerRsp StatusGrpcClient::GetChatServer(int uid)
{
    ClientContext context;
    GetChatServerReq request;
    GetChatServerRsp reply;

    request.set_uid(uid);
    auto stub = _pool->GetConnection();

    Status status = stub->GetChatServer(&context, request, &reply);

    LOG_RPC->info("get chat server, status is {}", status.ok());

    Defer defer([&stub, this]
    {
        _pool->returnConnection(std::move(stub));
    });

    if(status.ok())
        return reply;
    else
    {
        reply.set_error(ErrorCodes::RPCFailed);
        return reply;
    }
}