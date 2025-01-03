#!/bin/bash

# 进入web目录
cd ../web || exit

# 安装依赖
echo "Installing dependencies..."
npm install

# 启动开发服务器
echo "Starting development server..."
npm run dev
