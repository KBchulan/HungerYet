#!/bin/bash

# 更新包列表
sudo apt-get update

# 安装依赖
sudo apt-get install -y curl gnupg apt-transport-https

# 添加RabbitMQ签名密钥
curl -1sLf "https://keys.openpgp.org/vks/v1/by-fingerprint/0A9AF2115F4687BD29803A206B73A36E6026DFCA" | sudo gpg --dearmor | sudo tee /usr/share/keyrings/com.rabbitmq.team.gpg > /dev/null

# 添加Erlang仓库
curl -1sLf https://dl.cloudsmith.io/public/rabbitmq/rabbitmq-erlang/gpg.E495BB49CC4BBE5B.key | sudo gpg --dearmor | sudo tee /usr/share/keyrings/rabbitmq.E495BB49CC4BBE5B.gpg > /dev/null
curl -1sLf https://dl.cloudsmith.io/public/rabbitmq/rabbitmq-erlang/config.deb.txt | sudo tee /etc/apt/sources.list.d/rabbitmq.list

# 添加RabbitMQ仓库
curl -1sLf https://dl.cloudsmith.io/public/rabbitmq/rabbitmq-server/gpg.9F4587F226208342.key | sudo gpg --dearmor | sudo tee /usr/share/keyrings/rabbitmq.9F4587F226208342.gpg > /dev/null
curl -1sLf https://dl.cloudsmith.io/public/rabbitmq/rabbitmq-server/config.deb.txt | sudo tee -a /etc/apt/sources.list.d/rabbitmq.list

# 更新包列表
sudo apt-get update

# 安装Erlang
sudo apt-get install -y erlang-base \
                        erlang-asn1 erlang-crypto erlang-eldap erlang-ftp erlang-inets \
                        erlang-mnesia erlang-os-mon erlang-parsetools erlang-public-key \
                        erlang-runtime-tools erlang-snmp erlang-ssl \
                        erlang-syntax-tools erlang-tftp erlang-tools erlang-xmerl

# 安装RabbitMQ服务器
sudo apt-get install -y rabbitmq-server

# 安装RabbitMQ C客户端库
sudo apt-get install -y librabbitmq-dev

# 启动RabbitMQ服务
sudo systemctl start rabbitmq-server
sudo systemctl enable rabbitmq-server

# 启用RabbitMQ管理插件
sudo rabbitmq-plugins enable rabbitmq_management

# 创建管理员用户（请修改用户名和密码）
sudo rabbitmqctl add_user admin admin123
sudo rabbitmqctl set_user_tags admin administrator
sudo rabbitmqctl set_permissions -p / admin ".*" ".*" ".*"

# 显示状态信息
echo "RabbitMQ安装完成！"
echo "管理界面: http://localhost:15672"
echo "默认用户名: admin"
echo "默认密码: admin123"
sudo systemctl status rabbitmq-server 