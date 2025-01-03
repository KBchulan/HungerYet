if [ -d "../logs" ]; then
    echo "正在删除 ../logs ..."
    rm -rf ../logs
fi

if [ -d "../client/build" ]; then
    echo "正在删除 ../client/build ..."
    rm -rf ../client/build
fi

if [ -d "../server/PurchaseServer1/build" ]; then
    echo "正在删除 ../server/PurchaseServer1/build ..."
    rm -rf ../server/PurchaseServer1/build
fi

if [ -d "../server/PurchaseServer2/build" ]; then
    echo "正在删除 ../server/PurchaseServer2/build ..."
    rm -rf ../server/PurchaseServer2/build
fi

if [ -d "../server/GateServer/build" ]; then
    echo "正在删除 ../server/GateServer/build ..."
    rm -rf ../server/GateServer/build
fi

if [ -d "../server/StatusServer/build" ]; then
    echo "正在删除 ../server/StatusServer/build ..."
    rm -rf ../server/StatusServer/build
fi

if [ -d "../server/VarifyServer/node_modules" ]; then
    echo "正在删除 ../server/VarifyServer/node_modules ..."
    rm -rf ../server/VarifyServer/node_modules
fi

if [ -d "../nginx/logs" ]; then
    echo "正在删除 ../nginx/logs ..."
    sudo rm -rf ../nginx/logs
fi

# 清理Web前端相关目录
if [ -d "../web/node_modules" ]; then
    echo "正在删除 ../web/node_modules ..."
    rm -rf ../web/node_modules
fi

if [ -d "../web/dist" ]; then
    echo "正在删除 ../web/dist ..."
    rm -rf ../web/dist
fi

if [ -d "../web/.vite" ]; then
    echo "正在删除 ../web/.vite ..."
    rm -rf ../web/.vite
fi

if [ -f "../web/npm-debug.log" ]; then
    echo "正在删除 ../web/npm-debug.log ..."
    rm -f ../web/npm-debug.log*
fi

if [ -f "../web/yarn-debug.log" ]; then
    echo "正在删除 ../web/yarn-debug.log ..."
    rm -f ../web/yarn-debug.log*
fi

if [ -f "../web/yarn-error.log" ]; then
    echo "正在删除 ../web/yarn-error.log ..."
    rm -f ../web/yarn-error.log*
fi

if [ -d "../web/coverage" ]; then
    echo "正在删除 ../web/coverage ..."
    rm -rf ../web/coverage
fi

# 检查是否有运行中的容器
if [ "$(docker ps -q)" ]; then
    echo "正在停止所有运行中的容器..."
    docker stop $(docker ps -aq)
fi

# 检查是否有任何容器或镜像需要清理
if [ "$(docker ps -aq)" ] || [ "$(docker images -q)" ]; then
    echo "正在清理 docker 系统..."
    docker system prune -a -f
    echo "docker 容器和镜像已清理完成"
else
    echo "没有需要清理的 docker 容器和镜像"
fi

echo "所有指定目录清理完成"

