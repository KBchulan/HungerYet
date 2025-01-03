#include "../include/rabbitmq_manager.h"
#include <stdexcept>
#include <iostream>

namespace hunger_yet
{
    namespace rabbitmq
    {

        Manager &Manager::getInstance()
        {
            static Manager instance;
            return instance;
        }

        Manager::Manager()
            : conn_(nullptr), isConnected_(false), host_(Config::getHost()), port_(Config::getPort()), username_(Config::getUsername()), password_(Config::getPassword()), vhost_(Config::getVHost())
        {
        }

        Manager::~Manager()
        {
            disconnect();
        }

        bool Manager::connect()
        {
            if (isConnected_)
            {
                return true;
            }

            conn_ = amqp_new_connection();
            if (!conn_)
            {
                handleError("创建连接失败");
                return false;
            }

            amqp_socket_t *socket = amqp_tcp_socket_new(conn_);
            if (!socket)
            {
                handleError("创建socket失败");
                return false;
            }

            int status = amqp_socket_open(socket, host_.c_str(), port_);
            if (status != AMQP_STATUS_OK)
            {
                handleError("打开socket失败");
                return false;
            }

            amqp_rpc_reply_t reply = amqp_login(conn_, vhost_.c_str(), 0, 131072, 0,
                                                AMQP_SASL_METHOD_PLAIN, username_.c_str(), password_.c_str());
            if (reply.reply_type != AMQP_RESPONSE_NORMAL)
            {
                handleError("登录失败");
                return false;
            }

            amqp_channel_open(conn_, 1);
            reply = amqp_get_rpc_reply(conn_);
            if (reply.reply_type != AMQP_RESPONSE_NORMAL)
            {
                handleError("打开通道失败");
                return false;
            }

            isConnected_ = true;
            return true;
        }

        void Manager::disconnect()
        {
            if (isConnected_ && conn_)
            {
                amqp_channel_close(conn_, 1, AMQP_REPLY_SUCCESS);
                amqp_connection_close(conn_, AMQP_REPLY_SUCCESS);
                amqp_destroy_connection(conn_);
                conn_ = nullptr;
                isConnected_ = false;
            }
        }

        bool Manager::publishMessage(const std::string &exchange,
                                     const std::string &routingKey,
                                     const std::string &message)
        {
            if (!checkConnection())
            {
                return false;
            }

            amqp_basic_properties_t props;
            props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
            props.content_type = amqp_cstring_bytes("text/plain");
            props.delivery_mode = 2; // 持久化消息

            int status = amqp_basic_publish(conn_, 1,
                                            amqp_cstring_bytes(exchange.c_str()),
                                            amqp_cstring_bytes(routingKey.c_str()),
                                            0, 0, &props,
                                            amqp_cstring_bytes(message.c_str()));

            if (status != AMQP_STATUS_OK)
            {
                handleError("发布消息失败");
                return false;
            }

            return true;
        }

        bool Manager::consumeMessage(const std::string &queue,
                                     std::function<void(const std::string &)> callback)
        {
            if (!checkConnection())
            {
                return false;
            }

            amqp_basic_consume(conn_, 1,
                               amqp_cstring_bytes(queue.c_str()),
                               amqp_empty_bytes, 0, 0, 0, amqp_empty_table);

            amqp_rpc_reply_t reply = amqp_get_rpc_reply(conn_);
            if (reply.reply_type != AMQP_RESPONSE_NORMAL)
            {
                handleError("设置消费者失败");
                return false;
            }

            while (true)
            {
                amqp_envelope_t envelope;
                amqp_maybe_release_buffers(conn_);

                reply = amqp_consume_message(conn_, &envelope, nullptr, 0);
                if (reply.reply_type != AMQP_RESPONSE_NORMAL)
                {
                    break;
                }

                std::string message(static_cast<char *>(envelope.message.body.bytes),
                                    envelope.message.body.len);
                callback(message);

                amqp_destroy_envelope(&envelope);
            }

            return true;
        }

        bool Manager::declareExchange(const std::string &exchange)
        {
            if (!checkConnection())
            {
                return false;
            }

            amqp_exchange_declare(conn_, 1,
                                  amqp_cstring_bytes(exchange.c_str()),
                                  amqp_cstring_bytes("topic"),
                                  0, 1, 0, 0, amqp_empty_table);

            amqp_rpc_reply_t reply = amqp_get_rpc_reply(conn_);
            if (reply.reply_type != AMQP_RESPONSE_NORMAL)
            {
                handleError("声明交换机失败");
                return false;
            }

            return true;
        }

        bool Manager::declareQueue(const std::string &queue)
        {
            if (!checkConnection())
            {
                return false;
            }

            amqp_queue_declare(conn_, 1,
                               amqp_cstring_bytes(queue.c_str()),
                               0, 1, 0, 0, amqp_empty_table);

            amqp_rpc_reply_t reply = amqp_get_rpc_reply(conn_);
            if (reply.reply_type != AMQP_RESPONSE_NORMAL)
            {
                handleError("声明队列失败");
                return false;
            }

            return true;
        }

        bool Manager::bindQueue(const std::string &queue,
                                const std::string &exchange,
                                const std::string &routingKey)
        {
            if (!checkConnection())
            {
                return false;
            }

            amqp_queue_bind(conn_, 1,
                            amqp_cstring_bytes(queue.c_str()),
                            amqp_cstring_bytes(exchange.c_str()),
                            amqp_cstring_bytes(routingKey.c_str()),
                            amqp_empty_table);

            amqp_rpc_reply_t reply = amqp_get_rpc_reply(conn_);
            if (reply.reply_type != AMQP_RESPONSE_NORMAL)
            {
                handleError("绑定队列失败");
                return false;
            }

            return true;
        }

        bool Manager::checkConnection()
        {
            if (!isConnected_)
            {
                return connect();
            }
            return true;
        }

        void Manager::handleError(const std::string &operation)
        {
            std::cerr << "RabbitMQ错误: " << operation << std::endl;
            if (conn_)
            {
                amqp_rpc_reply_t reply = amqp_get_rpc_reply(conn_);
                if (reply.reply_type != AMQP_RESPONSE_NORMAL)
                {
                    std::cerr << "错误详情: " << amqp_error_string2(reply.library_error) << std::endl;
                }
            }
        }

    } // namespace rabbitmq
} // namespace hunger_yet