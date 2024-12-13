#ifndef LOGICSYSTEM_H
#define LOGICSYSTEM_H

#include "const.h"
#include "MysqlManager.h"

#include <map>
#include <queue>
#include <thread>
#include <condition_variable>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/reader.h>

class CSession;
class LogicNode;
class LogicSystem
{
    using FunCallBack = std::function<void(std::shared_ptr<CSession> session, const short &msg_id, const std::string &msg_data)>;

public:
    ~LogicSystem();

    LogicSystem(const LogicSystem &) = delete;
    LogicSystem &operator=(const LogicSystem &) = delete;

    // 将信息投递到队列里
    void PostMsgToQue(std::shared_ptr<LogicNode> msg);

    // 获取实例
    static LogicSystem &GetInstance();

private:
    LogicSystem();

    // 逻辑线程函数
    void DealMsg();

    // 所有回调集合
    void RegisterCallBacks();

    // 登录回调
    void LoginHandler(std::shared_ptr<CSession> session, const short &id, const std::string &data);

private:
    std::mutex _mutex;
    std::atomic<bool> _stop;
    std::thread _worker_thread;
    std::condition_variable _consume;
    std::map<short, FunCallBack> _fun_callbacks;
    std::map<int, std::shared_ptr<UserInfo>> _users;
    std::queue<std::shared_ptr<LogicNode>> _msg_que;
};

#endif // !LOGICSYSTEM_H