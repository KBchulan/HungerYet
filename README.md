# HungerYet

![esu](client/resources/icos/esu.png)

## 高级程序设计课设:  中南饿了么

## 项目简介

HungerYet是一个基于C++和Node.js开发的校园外卖订餐系统,采用分布式微服务架构设计。

### 系统架构

系统采用前后端分离的微服务架构:

#### 前端

- 使用Qt5开发的跨平台桌面客户端
- OpenCV实现二维码扫描功能
- 支持两种主题切换的现代化UI设计

#### 后端微服务

- GateServer (50001): API网关服务
- VerifyServer (50002): 邮箱验证服务
- StatusServer (50003): 状态管理服务
- PurchaseServer1 (50004): 订单处理服务1
- PurchaseServer2 (50005): 订单处理服务2

### 主要功能

- 用户注册/登录(支持邮箱验证)
- 商家信息浏览
- 菜品订购
- 订单管理
- 二维码扫描点餐
- 智能客服对话

## 技术栈

### 客户端
- C++20
- Qt 5.14.2
- OpenCV
- libcurl
- jsoncpp

### 服务端
- C++20
- Node.js
- gRPC
- Redis
- MySQL
- Boost
- spdlog
- hiredis

## 构建与运行

### 环境要求
- GCC 9.4+
- Clang 14.0.0+
- CMake 3.24+
- Qt 5.14.2+
- Node.js 22.11+
- OpenCV 4+
- Boost 1.82.0+
- protobuf 3.13+
- gRPC 1.50.0+
- hiredis 1.0.3+
- spdlog 1.10.0+
- jsoncpp 1.9.5+
- libcurl 7.80.0+
- MySQL 8.0.34+
- Redis 6.0.16+

### 编译运行
```bash
cd scripts

# 服务器
./aVarify.sh
./bGateServerCompile.sh
./bStatusCompile.sh
./b1PucharseServerCompile.sh
./b2PucharseServerCompile.sh

# 客户端
./bCmakeClientCompile.sh
```

## 许可证

本项目采用 MIT 许可证,详见 [LICENSE](LICENSE) 文件。

## 作者
KBchulan
fgloris