# HungerYet

HungerYet 是一个现代化的餐饮服务系统，采用微服务架构设计，提供完整的用户认证、状态管理、订单处理等功能。系统使用 C++ 开发后端服务，Qt 开发客户端，并集成了 Nginx 反向代理、RabbitMQ 消息队列和 Swagger API 文档。现在还提供了基于 Vue + ts 的 Web 前端界面，为用户提供更便捷的访问方式。

## 系统架构

系统采用微服务架构，各服务通过 gRPC 进行同步通信，通过 RabbitMQ 进行异步消息传递，使用 Redis 进行缓存，MySQL 存储持久化数据。

### 1. API 网关 (GateServer)

- 统一的 API 入口点，处理所有外部请求
- 请求路由和负载均衡
- 用户认证和授权管理
- 请求限流和熔断保护
- 数据库操作（MySQL）
- 缓存管理（Redis）
- 系统日志发送到 RabbitMQ
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

### 7. Web 前端 (Vue.ts)

- 基于 Vue 3 和 TypeScript 开发
- 响应式设计，支持多设备访问
- 实时购物车管理
- 订单状态追踪
- 用户认证和授权
- 主要端口：5174

### 8. 消息队列 (RabbitMQ)

- 订单状态更新通知
- 系统日志收集
- 邮件通知队列
- 服务间异步通信
- 消息持久化
- 主要端口：5672 (AMQP)
- 管理界面端口：15672

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
- RabbitMQ 3.10.0+ (消息队列)
- Docker & Docker Compose (容器化)

### Web 前端依赖

- Node.js 16+
- npm 8+
- Vue 3
- TypeScript 5+
- Vite 6+
- Vue Router 4+
- Axios

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
sudo apt install -y swagger
```

### 2. 克隆项目

```bash
git clone https://github.com/KBchulan/HungerYet.git
cd HungerYet
```

### 3. 启动服务（按顺序）# 1. 启动 VerifyServer（验证服务）

```bash
cd scripts
./aVarify.sh

# 2. 启动 GateServer（API网关）
./bGateServerCompile.sh

# 3. 启动 StatusServer（状态服务）
./bStatusCompile.sh

# 4. 启动 PurchaseServer（购买服务）
./b2PucharseServerCompile.sh

# 5. 启动 Web 前端开发服务器
./aWeb.sh

# 6. 启动 Docker 服务（Nginx 和 Swagger UI）
./bDocker.sh

# 7. web client
./aWeb.sh

# 8. qt client
./bCmakeClientCompile.sh
```

### 4. 验证服务

- 访问 http://localhost:8080 查看 API 网关状态页面
- 访问 http://localhost:8081 查看 Swagger API 文档
- 访问 http://localhost:5174 使用 Web 前端界面

## 项目结构

```
HungerYet/
├── client/           # Qt客户端代码
│   ├── src/         # 源代码
│   ├── include/     # 头文件
│   └── ui/          # 界面文件
├── web/             # Web前端代码
│   ├── src/         # 源代码
│   ├── public/      # 静态资源
│   └── dist/        # 构建输出
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

### Web 前端开发规范

- 使用 TypeScript 进行类型检查
- 遵循 Vue 3 组合式 API 风格
- 使用 ESLint 和 Prettier 保持代码风格一致
- 组件化开发，提高代码复用性
- 响应式设计，适配多种设备

### 测试

- 单元测试：GTest
- 集成测试：自动化测试脚本
- 性能测试：压力测试工具
- Web 前端测试：Vitest

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
