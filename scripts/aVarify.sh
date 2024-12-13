VARIFY_DIR="../server/VarifyServer"
cd "$VARIFY_DIR"

if [ ! -d "node_modules" ]; then
    echo "正在安装依赖"
    npm install
    
    if [ $? -ne 0 ]; then
        echo "依赖安装失败!"
        exit 1
    fi
    echo "依赖安装成功!"
fi

echo "正在启动 VarifyServer"
node server.js

if [ $? -ne 0 ]; then
    echo "VarifyServer 启动失败!"
    exit 1
fi