BUILD_DIR="../server/PurchaseServer1/build"

if [ -d "$BUILD_DIR" ]; then
    echo "检测到build文件夹已存在,正在删除"
    rm -rf "$BUILD_DIR"
fi

mkdir "$BUILD_DIR"
cd "$BUILD_DIR"

cmake ..
make -j$(nproc)

if [ $? -eq 0 ]; then
    echo "编译成功,正在启动程序..."
    ./bin/PurchaseServer1
else
    echo "编译失败!"
    exit 1
fi
