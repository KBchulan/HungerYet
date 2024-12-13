#ifndef STATUSGRPCCLIENT_H
#define STATUSGRPCCLIENT_H

#include "const.h"
#include "message.pb.h"
#include "ConfigManager.h"
#include "message.grpc.pb.h"

#include <queue>
#include <grpcpp/grpcpp.h>
#include <condition_variable>

using grpc::Status;
using grpc::Channel;
using grpc::ClientContext;

using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::LoginReq;
using message::LoginRsp;
using message::StatusService;

class StatusConPool
{
public:
    StatusConPool(std::uint32_t size, std::string host, std::string port);

    ~StatusConPool();

    std::unique_ptr<StatusService::Stub> GetConnection();

    void returnConnection(std::unique_ptr<StatusService::Stub> context);

    void Close();

private:
    std::mutex _mutex;
    std::string _host;
    std::string _port;
    std::uint32_t _poolSize;
    std::atomic_bool _b_stop;
    std::condition_variable _cond;
    std::queue<std::unique_ptr<StatusService::Stub>> _connections;
};

class StatusGrpcClient final : public Singleton<StatusGrpcClient>
{
    friend class Singleton<StatusGrpcClient>;

public:
    ~StatusGrpcClient();

    GetChatServerRsp GetChatServer(int uid);

    LoginRsp Login(int uid, std::string token);

private:
    StatusGrpcClient();
    
private:
    std::unique_ptr<StatusConPool> _pool;
};

#endif // !STATUSGRPCCLIENT_H