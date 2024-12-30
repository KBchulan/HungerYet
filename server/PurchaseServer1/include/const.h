#ifndef CONST_H
#define CONST_H

#include <functional>

class Defer
{
public:
    Defer(std::function<void()> func) : _func(func) {}

    ~Defer() { _func(); }

private:
    std::function<void()> _func;
};

#define MAX_LENGTH 1024 * 4    // 单个包的最大长度
#define HEAD_TOTAL_LEN 4       // 头部总长度
#define HEAD_ID_LEN 2          // ID长度
#define HEAD_DATA_LEN 2        // 信息长度(不是信息)
#define MAX_RECVQUE 10000      // 接收队列最大值
#define MAX_SENDQUE 1000       // 发送队列最大值

enum MSG_IDS
{
    MSG_CHAT_LOGIN = 1005,              // 用户登录
    MSG_CHAT_LOGIN_RSP = 1006,          // 用户登录回包
    MSG_PURCHASE = 1007,                // 用户购买
    MSG_PURCHASE_RSP = 1008,            // 用户购买回包
    MSG_GET_ORDERS = 1009,              // 获取订单
    MSG_GET_ORDERS_RSP = 1010,          // 获取订单回包
    MSG_ADMIN_GET_ORDERS = 1011,        // 管理员获取订单
    MSG_ADMIN_GET_ORDERS_RSP = 1012,    // 管理员获取订单回包
    MSG_UPDATE_ORDER_STATUS = 1013,     // 更新订单状态
    MSG_UPDATE_ORDER_STATUS_RSP = 1014, // 更新订单状态回包
};

enum ErrorCodes
{
    Success = 0,            // 成功
    ErrorJson = 1001,       // JSON解析错误
    RPCFailed = 1002,       // RPC调用失败
    VarifyExpired = 1003,   // 验证码过期
    VarifyCodeErr = 1004,   // 验证码错误
    UserExist = 1005,       // 用户已存在
    PasswdErr = 1006,       // 密码错误
    EmailNotMatch = 1007,   // 邮箱不匹配
    PasswdUpFailed = 1008,  // 密码更新失败
    PasswdInvalid = 1009,   // 密码格式无效
    TokenInvalid = 1010,    // token无效
    UidInvaild = 1011,      // uid无效
    DBError = 1012,         // 数据库错误
};

#endif // !CONST_H