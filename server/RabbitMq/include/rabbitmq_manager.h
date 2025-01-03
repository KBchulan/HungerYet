#ifndef HUNGER_YET_RABBITMQ_MANAGER_H
#define HUNGER_YET_RABBITMQ_MANAGER_H

#include <memory>
#include <string>
#include <functional>
#include <amqp.h>
#include <amqp_tcp_socket.h>

#include "rabbitmq_config.h"

namespace hunger_yet
{
    namespace rabbitmq
    {

        class Manager
        {
        public:
            static Manager &getInstance();

            bool connect();
            void disconnect();

            // 发布消息
            bool publishMessage(const std::string &exchange,
                                const std::string &routingKey,
                                const std::string &message);

            // 消费消息
            bool consumeMessage(const std::string &queue,
                                std::function<void(const std::string &)> callback);

            // 声明交换机
            bool declareExchange(const std::string &exchange);

            // 声明队列
            bool declareQueue(const std::string &queue);

            // 绑定队列到交换机
            bool bindQueue(const std::string &queue,
                           const std::string &exchange,
                           const std::string &routingKey);

        private:
            Manager();
            ~Manager();

            // 禁止拷贝和赋值
            Manager(const Manager &) = delete;
            Manager &operator=(const Manager &) = delete;

            // 检查连接状态
            bool checkConnection();

            // 处理错误
            void handleError(const std::string &operation);

        private:
            amqp_connection_state_t conn_;
            bool isConnected_;

            // 连接参数
            std::string host_;
            int port_;
            std::string username_;
            std::string password_;
            std::string vhost_;
        };

    } // namespace rabbitmq
} // namespace hunger_yet

#endif // HUNGER_YET_RABBITMQ_MANAGER_H