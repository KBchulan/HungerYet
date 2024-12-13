#include "../include/CSession.h"
#include "../include/LogManager.h"
#include "../include/LogicSystem.h"
#include "../include/StatusGrpcClient.h"

LogicSystem::LogicSystem() 
    : _stop(false)
{
    RegisterCallBacks();

    _worker_thread = std::thread(&LogicSystem::DealMsg, this);
}

LogicSystem::~LogicSystem()
{
    _stop = true;
    _consume.notify_one();
    _worker_thread.join();
}

void LogicSystem::PostMsgToQue(std::shared_ptr<LogicNode> msg)
{
    std::unique_lock<std::mutex> lock(_mutex);
    _msg_que.push(msg);

    if(_msg_que.size() == 1)
    {
        _consume.notify_one();
    }
}

LogicSystem &LogicSystem::GetInstance()
{
    static LogicSystem instance;
    return instance;
}

void LogicSystem::DealMsg()
{
    while(true)
    {
        std::unique_lock<std::mutex> lock(_mutex);

        while(_msg_que.empty() && !_stop)
        {
            _consume.wait(lock);
        }

        if (_stop)
        {
            while(!_msg_que.empty())
            {
                auto msg_node = _msg_que.front();
                LOG_SERVER->info("recv msg id is: {}", msg_node->_recvnode->GetMsgId());

                if(_fun_callbacks.find(msg_node->_recvnode->GetMsgId()) == _fun_callbacks.end())
                {
                    _msg_que.pop();
                    continue;
                }

                _fun_callbacks.find(msg_node->_recvnode->GetMsgId())->second(msg_node->_session, msg_node->_recvnode->GetMsgId(), std::string(msg_node->_recvnode->_data, msg_node->_recvnode->_total_len));
                _msg_que.pop();
            }
            break;
        }

        // 有数据传入
        auto msg_node = _msg_que.front();
        LOG_SERVER->info("recv msg id is: {}", msg_node->_recvnode->GetMsgId());
        if(_fun_callbacks.find(msg_node->_recvnode->GetMsgId()) == _fun_callbacks.end())
        {
            _msg_que.pop();
            continue;
        }
        
        _fun_callbacks.find(msg_node->_recvnode->GetMsgId())->second(msg_node->_session, msg_node->_recvnode->GetMsgId(), std::string(msg_node->_recvnode->_data, msg_node->_recvnode->_total_len));
        _msg_que.pop();
    }
}

void LogicSystem::RegisterCallBacks()
{
    _fun_callbacks[MSG_CHAT_LOGIN] = [this](std::shared_ptr<CSession> session, const short &id, const std::string &data) 
    {
        LoginHandler(session, id, data);
    };
}

void LogicSystem::LoginHandler(std::shared_ptr<CSession> session, const short &id, const std::string &data)
{
    Json::Reader reader;
    Json::Value root;

    reader.parse(data, root);

    auto uid = root["uid"].asInt();
    LOG_SERVER->info("user login, uid is: {}", uid);
    LOG_SERVER->info("user login, token is: {}", root["token"].asString());

    auto rsp = StatusGrpcClient::GetInstance()->Login(uid, root["token"].asString());

    Json::Value rtvalue;

    Defer defer([this, &rtvalue, session]() -> void
    {
        std::string return_str = rtvalue.toStyledString();
        session->Send(return_str, MSG_CHAT_LOGIN_RSP);
    });

    rtvalue["error"] = rsp.error();

    if(rsp.error() != ErrorCodes::Success)
        return;
    
    auto find_iter = _users.find(uid);
    std::shared_ptr<UserInfo> user_info = nullptr;
    if(find_iter == _users.end())
    {
        user_info = MysqlManager::GetInstance()->GetUser(uid);

        if(user_info == nullptr)
        {
            rtvalue["error"] = ErrorCodes::UidInvaild;
            return;
        }
        _users[uid] = user_info;
    }
    else
    {
        user_info = find_iter->second;
    }

    rtvalue["uid"] = uid;
    rtvalue["token"] = rsp.token();
    rtvalue["name"] = user_info->_name;
}
