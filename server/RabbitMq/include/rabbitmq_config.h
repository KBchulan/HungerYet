#ifndef HUNGER_YET_RABBITMQ_CONFIG_H
#define HUNGER_YET_RABBITMQ_CONFIG_H

#include <string>

namespace hunger_yet
{
    namespace rabbitmq
    {

        class Config
        {
        public:
            static std::string getHost() { return "localhost"; }
            static int getPort() { return 5672; }
            static std::string getUsername() { return "guest"; }
            static std::string getPassword() { return "guest"; }
            static std::string getVHost() { return "/"; }

            // 队列名称定义
            class Queues
            {
            public:
                static std::string getOrderQueue() { return "hunger_yet_orders"; }
                static std::string getEmailQueue() { return "hunger_yet_emails"; }
                static std::string getStatusQueue() { return "hunger_yet_status"; }
                static std::string getLogQueue() { return "hunger_yet_logs"; }
            };

            // 交换机名称定义
            class Exchanges
            {
            public:
                static std::string getOrderExchange() { return "hunger_yet_order_exchange"; }
                static std::string getEmailExchange() { return "hunger_yet_email_exchange"; }
                static std::string getStatusExchange() { return "hunger_yet_status_exchange"; }
                static std::string getLogExchange() { return "hunger_yet_log_exchange"; }
            };

            // 路由键定义
            class RoutingKeys
            {
            public:
                static std::string getOrderCreate() { return "order.create"; }
                static std::string getOrderUpdate() { return "order.update"; }
                static std::string getEmailVerify() { return "email.verify"; }
                static std::string getStatusUpdate() { return "status.update"; }
                static std::string getLogInfo() { return "log.info"; }
                static std::string getLogError() { return "log.error"; }
            };
        };

    } // namespace rabbitmq
} // namespace hunger_yet

#endif // HUNGER_YET_RABBITMQ_CONFIG_H