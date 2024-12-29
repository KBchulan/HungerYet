#include "../include/LogManager.h"
#include "../include/LogicSystem.h"
#include "../include/RedisManager.h"
#include "../include/MysqlManager.h"
#include "../include/HttpConnection.h"
#include "../include/StatusGrpcClient.h"
#include "../include/VerifyGrpcClient.h"
#include "LogicSystem.h"

LogicSystem::~LogicSystem()
{
    LOG_SERVER->info(R"(LogicSystem has been destructed!)");
}

LogicSystem::LogicSystem()
{
    RegGet("/get_test", [](std::shared_ptr<HttpConnection> connection)
    {
        boost::beast::ostream(connection->_response.body()) << "receive get_test req" << '\n';
        int i = 0;
        for(auto &elem : connection->_get_params)
        {
            i++;
            boost::beast::ostream(connection->_response.body())
                << "params" << i << ": key is " << elem.first 
                << ", value is " << elem.second << '\n';
        }
    });

    RegPost("/get_varifycode", [](std::shared_ptr<HttpConnection> connection)
    {
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
        LOG_SERVER->info(R"({} : {})", __FILE__, "receive body is: " + body_str);

        connection->_response.set(boost::beast::http::field::content_type, "text/json");
        
        Json::Value dst_root;
        Json::Reader reader;
        Json::Value src_root;

        bool parse_success = reader.parse(body_str, src_root);
        if(!parse_success)
        {
            LOG_SERVER->error(R"({} : {})", __FILE__, "Failed to parse Json data");
            dst_root["error"] = ErrorCodes::ErrorJson;
            std::string jsonstr = dst_root.toStyledString();
            boost::beast::ostream(connection->_response.body()) << jsonstr << '\n'; 
            return true;
        }

        if(src_root.isMember("email"))
        {
            auto email = src_root["email"].asString();
            GetVarifyRsp rsp = VerifyGrpcClient::GetInstance()->GetVarifyCode(email);
            dst_root["error"] = rsp.error();
            dst_root["email"] = src_root["email"];
            std::string jsonstr = dst_root.toStyledString();
            boost::beast::ostream(connection->_response.body()) << jsonstr; 
            return true;
        }
        else
        {
            LOG_SERVER->error(R"({} : {})", __FILE__, "Failed to parse Json data");
            dst_root["error"] = ErrorCodes::ErrorJson;
            std::string jsonstr = dst_root.toStyledString();
            boost::beast::ostream(connection->_response.body()) << jsonstr; 
            return true;
        }
    });

    RegPost("/user_register", [](std::shared_ptr<HttpConnection> connection)
    {
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
        LOG_SERVER->info(R"({} : {})", __FILE__, "receive body is: " + body_str);

        connection->_response.set(boost::beast::http::field::content_type, "text/json");
        
        Json::Value dst_root;
        Json::Reader reader;
        Json::Value src_root;

        bool parse_success = reader.parse(body_str, src_root);
        if(!parse_success)
        {
            LOG_SERVER->error(R"({} : {})", __FILE__, "Failed to parse Json data");
            dst_root["error"] = ErrorCodes::ErrorJson;
            std::string jsonstr = dst_root.toStyledString();
            boost::beast::ostream(connection->_response.body()) << jsonstr; 
            return true;
        }

        // 输入密码和确认是否一样
        if(src_root["passwd"].asString() != src_root["confirm"].asString())
        {
            LOG_SERVER->error(R"({} : {})", __FILE__, "confirm not equal to passwd");
            dst_root["error"] = ErrorCodes::PasswdErr;
            std::string jsonstr = dst_root.toStyledString();
            boost::beast::ostream(connection->_response.body()) << jsonstr; 
            return true;
        }

        std::string varify_code;
        bool b_get_varify = RedisManager::GetInstance()->Get(CODEPREFIX + src_root["email"].asString(), varify_code);
        
        // 验证码过期
        if(!b_get_varify)
        {
            LOG_SERVER->error(R"({} : {})", __FILE__, "get varify code expired");
            dst_root["error"] = ErrorCodes::VarifyExpired;
            std::string jsonstr = dst_root.toStyledString();
            boost::beast::ostream(connection->_response.body()) << jsonstr; 
            return true;
        }

        // 验证码不匹配
        if(varify_code != src_root["varifycode"].asString())
        {
            LOG_SERVER->error(R"({} : {})", __FILE__, "varify code error");
            dst_root["error"] = ErrorCodes::VarifyCodeErr;
            std::string jsonstr = dst_root.toStyledString();
            boost::beast::ostream(connection->_response.body()) << jsonstr; 
            return true;
        }
        
        // mysql查找user
        int uid = MysqlManager::GetInstance()->RegUser(src_root["user"].asString(), src_root["email"].asString(), src_root["passwd"].asString());
        if(uid == 0 || uid == -1)
        {
            LOG_SERVER->error(R"({} : {})", __FILE__, "user or email exist");
            dst_root["error"] = ErrorCodes::UserExist;
            std::string jsonstr = dst_root.toStyledString();
            boost::beast::ostream(connection->_response.body()) << jsonstr; 
            return true;
        }
        
        // 回包
        dst_root["error"] = 0;
        dst_root["uid"] = uid;
        dst_root["email"] = src_root["email"].asString();
        dst_root ["user"]= src_root["user"].asString();
        dst_root["passwd"] = src_root["passwd"].asString();
        dst_root["confirm"] = src_root["confirm"].asString();
        dst_root["varifycode"] = src_root["varifycode"].asString();
        std::string jsonstr = dst_root.toStyledString();
        boost::beast::ostream(connection->_response.body()) << jsonstr;
        return true;
    });

    RegPost("/reset_pwd", [](std::shared_ptr<HttpConnection> connection)
    {
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
        LOG_SERVER->info(R"({} : {})", __FILE__, "receive body is: " + body_str);

        connection->_response.set(boost::beast::http::field::content_type, "text/json");

        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;

        bool parse_success = reader.parse(body_str, src_root);

        if(!parse_success)
        {
            LOG_HTTP->error(R"({} : {})", __FILE__, "Failed to parse Json data");
            root["error"] = ErrorCodes::ErrorJson;
            std::string jsonstr = root.toStyledString();
            boost::beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        auto name = src_root["user"].asString();
        auto email = src_root["email"].asString();
        auto passwd = src_root["passwd"].asString();
        auto varifycode = src_root["varifycode"].asString();

        // 先查看验证码是否存在
        std::string varify_code;
        bool b_get_varify = RedisManager::GetInstance()->Get(CODEPREFIX + email, varify_code);

        if(!b_get_varify)
        {
            LOG_HTTP->error(R"({} : {})", __FILE__, "get varify code expired");
            root["error"] = ErrorCodes::VarifyExpired;
            std::string jsonstr = root.toStyledString();
            boost::beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        // 验证码不匹配
        if(varify_code != varifycode)
        {
            LOG_HTTP->error(R"({} : {})", __FILE__, "varify code error");
            root["error"] = ErrorCodes::VarifyCodeErr;
            std::string jsonstr = root.toStyledString();
            boost::beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        // 查询数据库中用户名和邮箱是否匹配
        bool email_valid = MysqlManager::GetInstance()->CheckEmail(name, email);
        if(!email_valid)
        {
            LOG_HTTP->error(R"({} : {})", __FILE__, "email not match");
            root["error"] = ErrorCodes::EmailNotMatch;
            std::string jsonstr = root.toStyledString();
            boost::beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        // 更新数据库中的密码
        bool b_update_pwd = MysqlManager::GetInstance()->UpdatePwd(name, passwd);
        if(!b_update_pwd)
        {
            LOG_HTTP->error(R"({} : {})", __FILE__, "update pwd failed");
            root["error"] = ErrorCodes::PasswdUpFailed;
            std::string jsonstr = root.toStyledString();
            boost::beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        // 回包
        root["error"] = 0;
        root["user"] = name;
        root["email"] = email;
        root["passwd"] = passwd;
        root["varifycode"] = varifycode;
        std::string jsonstr = root.toStyledString();
        LOG_HTTP->info(R"({} : {})", __FILE__, "send response: " + jsonstr);
        boost::beast::ostream(connection->_response.body()) << jsonstr;
        return true;
    });

    RegPost("/user_login", [](std::shared_ptr<HttpConnection> connection)
    {
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
        LOG_HTTP->info(R"({} : {})", __FILE__, "receive body is: " + body_str);

        connection->_response.set(boost::beast::http::field::content_type, "text/json");

        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;

        bool parse_success = reader.parse(body_str, src_root);
        if(!parse_success)
        {
            LOG_HTTP->error(R"({} : {})", __FILE__, "Failed to parse Json data");
            root["error"] = ErrorCodes::ErrorJson;
            std::string jsonstr = root.toStyledString();
            boost::beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        auto email = src_root["email"].asString();
        auto passwd = src_root["passwd"].asString();

        UserInfo user_info;
        bool pwd_valid = MysqlManager::GetInstance()->CheckPwd(email, passwd, user_info);
        if(!pwd_valid)
        {
            LOG_HTTP->error(R"({} : {})", __FILE__, "passwd error");
            root["error"] = ErrorCodes::PasswdInvalid;
            std::string jsonstr = root.toStyledString();
            boost::beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }
        
        // 状态服务
        auto reply = StatusGrpcClient::GetInstance()->GetChatServer(user_info._uid);
        if(reply.error() != 0)
        {
            LOG_HTTP->error(R"({} : {})", __FILE__, "Grpc get chat server failed");
            root["error"] = ErrorCodes::RPCFailed;
            std::string jsonstr = root.toStyledString();
            boost::beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        root["error"] = 0;
        root["email"] = email;
        root["uid"] = user_info._uid;
        root["token"] = reply.token();
        root["host"] = reply.host();
        root["port"] = reply.port();
        std::string jsonstr = root.toStyledString();
        boost::beast::ostream(connection->_response.body()) << jsonstr;
        LOG_HTTP->info(R"({} : {})", __FILE__, "send response: " + jsonstr);
        return true;
    });
}

void LogicSystem::RegGet(const std::string &url, HttpHandler handler)
{
    _get_handlers.insert(std::make_pair(url, handler));
}

bool LogicSystem::HandleGet(const std::string &path, std::shared_ptr<HttpConnection> con)
{
    if(_get_handlers.find(path) == _get_handlers.end())
    {
        return false;
    }

    _get_handlers[path](con);
    return true;
}

void LogicSystem::RegPost(const std::string &url, HttpHandler handler)
{
    _post_handlers.insert(std::make_pair(url, handler));
}

bool LogicSystem::HandlePost(const std::string &path, std::shared_ptr<HttpConnection> con)
{
    if(_post_handlers.find(path) == _post_handlers.end())
    {
        return false;
    }

    _post_handlers[path](con);
    return true;
}
