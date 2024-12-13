#ifndef STATUSSERVICEIMPL_H
#define STATUSSERVICEIMPL_H

#include "const.h"
#include "message.grpc.pb.h"

#include <grpcpp/grpcpp.h>

using grpc::Server;
using grpc::Status;
using grpc::ServerBuilder;
using grpc::ServerContext;

using message::LoginReq;
using message::LoginRsp;
using message::GetChatServerReq;
using message::GetChatServerRsp;

using message::StatusService;

struct ChatServer
{
    int con_count;
    std::string host;
    std::string port;
    std::string name;
};

class StatusServiceImpl final : public StatusService::Service
{
public:
	StatusServiceImpl();

	Status GetChatServer(ServerContext* context, const GetChatServerReq* request, GetChatServerRsp* reply) override;

	Status Login(ServerContext* context, const LoginReq* request, LoginRsp* reply) override;

private:
	void insertToken(int uid, std::string token);

    ChatServer getChatServer();

	std::mutex _token_mtx;
    std::mutex _server_mtx;
    std::unordered_map<int, std::string> _tokens;
    std::unordered_map<std::string, ChatServer> _servers;
};

#endif // !STATUSSERVICEIMPL_H
