# HungerYet

HungerYet 是一个现代化的餐饮服务系统，采用微服务架构设计，提供完整的用户认证、状态管理、订单处理等功能。系统使用 C++ 开发后端服务，Qt 开发客户端，并集成了 Nginx 反向代理和 Swagger API 文档。

## 系统架构

系统采用微服务架构，各服务通过 gRPC 进行通信，使用 Redis 进行缓存，MySQL 存储持久化数据。

### 1. API 网关 (GateServer)

- 统一的 API 入口点，处理所有外部请求
- 请求路由和负载均衡
- 用户认证和授权管理
- 请求限流和熔断保护
- 数据库操作（MySQL）
- 缓存管理（Redis）
- 主要端口：50001

### 2. 验证服务 (VerifyServer)

- 邮箱验证码生成和发送
- 验证码有效期管理
- 验证码验证和校验
- Redis 缓存集成
- 邮件服务集成
- 主要端口：50002

### 3. 状态服务 (StatusServer)

- 用户会话管理
- 在线状态追踪
- 服务健康检查
- 负载均衡策略
- 实时状态同步
- 主要端口：50003

### 4. 购买服务 (PurchaseServer)

- 订单处理和管理
- 支付系统集成
- 库存管理和同步
- 订单状态追踪
- 双实例部署
- 主要端口：50004, 50005

### 5. Nginx 反向代理

- 请求转发和路由
- 负载均衡
- 静态资源服务
- SSL/TLS 终止
- 访问日志记录
- 主要端口：8080

### 6. Swagger UI

- RESTful API 文档
- 接口在线测试
- 请求/响应示例
- 错误码说明
- 主要端口：8081

## 开发环境要求

### 基础环境

- 操作系统：Ubuntu 22.04+
- 编译器：GCC 9.4+ 或 Clang 14.0.0+
- 构建工具：CMake 3.24+

### 依赖库

- Qt 5.14.2+ (GUI框架)
- libcurl 7.80.0+ (网络请求)
- OpenCV 4+ (图像处理)
- Node.js 22.11+ (运行环境)
- hiredis 1.0.3+ (Redis客户端)
- spdlog 1.10.0+ (日志库)
- jsoncpp 1.9.5+ (JSON解析)
- MySQL 8.0.34+ (数据库)
- Redis 6.0.16+ (缓存)
- Boost 1.82.0+ (基础库)
- protobuf 3.13+ (序列化)
- gRPC 1.50.0+ (RPC框架)
- Docker & Docker Compose (容器化)

## 快速开始

### 1. 环境准备

```bash
# 安装基础依赖
sudo apt update
sudo apt install -y build-essential cmake git

# 安装开发库
sudo apt install -y \
    libboost-all-dev \
    libmysqlclient-dev \
    libhiredis-dev \
    libgrpc++-dev \
    libprotobuf-dev \
    protobuf-compiler-grpc \
    libjsoncpp-dev \
    libspdlog-dev

# 其实还有一个swagger，作者使用go install了，读者可以选用apt
```

### 2. 克隆项目

```bash
git clone https://github.com/KBchulan/HungerYet.git
cd HungerYet
```

### 3. 启动服务（按顺序）

```bash
# 1. 启动 VerifyServer（验证服务）
cd scripts
./aVarify.sh

# 2. 启动 GateServer（API网关）
./bGateServerCompile.sh

# 3. 启动 StatusServer（状态服务）
./bStatusCompile.sh

# 4. 启动 PurchaseServer（购买服务）
./b2PucharseServerCompile.sh

# 5. 启动 Docker 服务（Nginx 和 Swagger UI）
cd ..
docker-compose up -d
```

### 4. 验证服务

- 访问 http://localhost:8080 查看 API 网关状态页面
- 访问 http://localhost:8081 查看 Swagger API 文档

## API 端点

### 用户认证

- `POST /api/get_varifycode` - 获取邮箱验证码

  - 请求：`{"email": "user@example.com"}`
  - 响应：`{"error": 0, "email": "user@example.com"}`
- `POST /api/user_register` - 用户注册

  - 请求：`{"user": "username", "email": "user@example.com", "passwd": "password", "varifycode": "123456"}`
  - 响应：`{"error": 0, "uid": 12345}`
- `POST /api/user_login` - 用户登录

  - 请求：`{"email": "user@example.com", "passwd": "password"}`
  - 响应：`{"error": 0, "uid": 12345, "token": "xxx"}`
- `POST /api/reset_pwd` - 重置密码

  - 请求：`{"email": "user@example.com", "varifycode": "123456", "passwd": "newpassword"}`
  - 响应：`{"error": 0}`

### 订单管理

- `POST /api/admin_get_orders` - 获取订单列表
  - 请求：`{}`
  - 响应：`{"error": 0, "orders": [...]}`

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

## 项目结构

```
HungerYet/
├── client/           # 客户端代码
│   ├── src/         # 源代码
│   ├── include/     # 头文件
│   └── ui/          # 界面文件
├── server/          # 服务器代码
│   ├── GateServer/  # API 网关服务
│   ├── VerifyServer/# 验证服务
│   ├── StatusServer/# 状态服务
│   └── PurchaseServer/# 购买服务
├── nginx/          # Nginx 配置
│   ├── nginx.conf  # 主配置文件
│   ├── html/       # 静态资源
│   └── logs/       # 日志文件
├── scripts/        # 启动和构建脚本
├── doc/           # 项目文档
├── tests/         # 测试代码
├── docker-compose.yaml # Docker 编排配置
└── README.md      # 项目说明
```

## 开发指南

### 编码规范

- 使用 C++20 标准
- 遵循 Google C++ 风格指南
- 使用智能指针管理内存
- 使用异常处理机制
- 添加适当的注释

### 测试

- 单元测试：GTest
- 集成测试：自动化测试脚本
- 性能测试：压力测试工具

### 日志

- 使用 spdlog 进行日志记录
- 日志级别：DEBUG, INFO, WARN, ERROR
- 日志轮转和归档

## 贡献指南

1. Fork 项目
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启 Pull Request

## 维护者

- [@](https://github.com/KBchulan)KBchulan
- [@](https://github.com/fgloris)fgloris

## 许可证

本项目采用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情
