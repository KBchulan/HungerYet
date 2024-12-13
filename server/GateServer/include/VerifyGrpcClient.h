/*
此处RPC Pool的队列管理优化：头尾各加一把锁（未完成）
*/

#ifndef VERIFYGRPCCLIENT_H
#define VERIFYGRPCCLIENT_H

#include "const.h"
#include "message.grpc.pb.h"

#include <grpcpp/grpcpp.h>

using grpc::Status;
using grpc::Channel;
using grpc::ClientContext;

using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;

class RPConPool
{
public:
    ~RPConPool();
    RPConPool(std::size_t poolsize, std::string host, std::string port);

    void Close();

    std::unique_ptr<VarifyService::Stub> GetConnection();
    
    void returnConnection(std::unique_ptr<VarifyService::Stub>);

private:
    std::mutex _mutex;
    std::string _host;
    std::string _port;
    std::size_t _poolSize;
    std::atomic_bool _b_stop;
    std::condition_variable _cond;
    std::queue<std::unique_ptr<VarifyService::Stub>> _connections;
};

class VerifyGrpcClient final : public Singleton<VerifyGrpcClient>
{
    friend class Singleton<VerifyGrpcClient>;

public:
    GetVarifyRsp GetVarifyCode(std::string email);

private:
    VerifyGrpcClient();

private:
    std::unique_ptr<RPConPool> _pool;
};

#endif // !VERIFYGRPCCLIENT_H