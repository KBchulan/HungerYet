#ifndef LOGICSYSTEM_H
#define LOGICSYSTEM_H

#include "const.h"

class HttpConnection;
using HttpHandler = std::function<void(std::shared_ptr<HttpConnection>)>;
class LogicSystem final : public Singleton<LogicSystem>
{
    friend class Singleton<LogicSystem>;

public:
    ~LogicSystem();

    // 处理get请求
    bool HandleGet(const std::string &, std::shared_ptr<HttpConnection>);

    // 注册get请求
    void RegGet(const std::string &, HttpHandler handler);

    // 处理post请求
    bool HandlePost(const std::string &, std::shared_ptr<HttpConnection>);

    // 注册post请求
    void RegPost(const std::string &, HttpHandler handler);

private:
    LogicSystem();

private:
    std::map<std::string, HttpHandler> _get_handlers;
    std::map<std::string, HttpHandler> _post_handlers;
};

#endif // !LOGICSYSTEM_H