# HungerYet

HungerYet是一个餐饮订单管理系统，包含用户端、商家端和管理员端。

## 系统架构

系统采用微服务架构，主要包含以下服务：

1. GateServer（网关服务器）

   - 处理HTTP请求
   - 用户认证和授权
   - 数据库操作（MySQL）
   - 缓存管理（Redis）
   - 日志记录
2. StatusServer（状态服务器）

   - 用户会话管理
   - 在线状态维护
   - Token验证
   - 服务发现
   - 负载均衡
   - 实时状态同步
3. PurchaseServer（购买服务器）

   - 订单处理和管理
   - 订单状态追踪
   - 商家订单分发
   - 数据统计
4. VerifyServer（验证服务器）

   - 邮箱验证码生成
   - 验证码发送
   - 验证码有效期管理
   - 验证码验证

## API文档

项目使用Swagger/OpenAPI提供API文档。

### 启动Swagger UI

1. 确保已安装Docker和Docker Compose：

```bash
sudo apt install docker.io docker-compose
```

2. 启动Swagger UI：

```bash
docker-compose up
```

3. 访问文档：

```
http://localhost:8081
```

### API列表

1. GET接口：

   - `/get_test` - 测试接口
2. POST接口：

   - `/get_varifycode` - 获取验证码
   - `/user_register` - 用户注册
   - `/reset_pwd` - 重置密码
   - `/user_login` - 用户登录
   - `/admin_get_orders` - 管理员获取订单

### 错误码说明

- 0: 成功
- 1: JSON解析错误
- 2: 数据库错误
- 1003: 验证码过期
- 1004: 验证码错误
- 1005: 用户已存在
- 1006: 密码错误
- 1007: 邮箱不匹配
- 1008: 密码更新失败
- 1009: 密码格式无效
- 1010: 数据库错误

## 开发环境

- 操作系统：Ubuntu 22.24
- 编译器：
  - GCC 9.4+
  - Clang 14.0.0+
- 构建工具：CMake 3.24+
- GUI框架：Qt 5.14.2+
- 数据库：MySQL 8.0.34+
- 缓存：Redis 6.0.16+
- 网络库：
  - Boost 1.82.0+
  - libcurl 7.80.0+
- RPC相关：
  - gRPC 1.50.0+
  - protobuf 3.13+
- 日志库：spdlog 1.10.0+
- JSON处理：jsoncpp 1.9.5+
- 图像处理：OpenCV 4+
- 运行时环境：Node.js 22.11+
- Redis客户端：hiredis 1.0.3+

## 目录结构

```
HungerYet/
├── client/           		# 客户端代码
├── server/           		# 服务器代码
│   ├── GateServer/   		# 网关服务器
│   ├── StatusServer/ 		# 状态服务器
│   └── PurchaseServer2/ 	# 购买服务器
├── doc/             		# 文档
├── scripts/         		# 脚本
├── swagger.yaml    		# API文档
└── docker-compose.yaml 	# Docker配置
```

## 构建和运行

1. 安装依赖：

```bash
# 安装必要的系统包，作者主体为源码安装，apt的话可能需要改一部分头文件
sudo apt install build-essential cmake git \
    libboost-all-dev \
    libmysqlclient-dev \
    libhiredis-dev \
    libgrpc++-dev \
    libprotobuf-dev \
    protobuf-compiler-grpc \
    libjsoncpp-dev \
    libspdlog-dev
```

2. 构建项目：

```bash
cd scripts

# 服务器编译及运行
./aVarify.sh
./bGateServerCompile.sh
./bStatusCompile.sh
./b2PucharseServerCompile.sh

# 客户端编译及运行
./bCmakeClientCompile.sh
```

## 贡献指南

1. Fork 项目
2. 创建功能分支
3. 提交更改
4. 推送到分支
5. 创建 Pull Request

## 许可证

[MIT License](LICENSE)
