if [ -d "../logs" ]; then
    echo "正在删除 ../logs ..."
    rm -rf ../logs
fi

if [ -d "../client/build" ]; then
    echo "正在删除 ../client/build ..."
    rm -rf ../client/qmake/build
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

echo "所有指定目录清理完成"

