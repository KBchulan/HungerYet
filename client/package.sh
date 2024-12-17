#!/bin/bash

# 创建构建目录
mkdir -p build
cd build

# 配置和构建项目
cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# 创建AppDir目录结构
mkdir -p AppDir/usr
make DESTDIR=$(pwd)/AppDir install

# 复制依赖库
mkdir -p AppDir/usr/lib
ldd AppDir/usr/bin/HungerYet | grep "=>" | awk '{print $3}' | grep -v "^/" | while read -r lib; do
    cp -L "$lib" AppDir/usr/lib/
done

# 复制Qt插件
mkdir -p AppDir/usr/plugins
cp -r /home/whx/qtware/5.14.2/gcc_64/plugins/platforms AppDir/usr/plugins/
cp -r /home/whx/qtware/5.14.2/gcc_64/plugins/imageformats AppDir/usr/plugins/

# 创建启动脚本
cat > AppDir/AppRun << 'EOF'
#!/bin/bash
HERE="$(dirname "$(readlink -f "${0}")")"
export LD_LIBRARY_PATH="${HERE}/usr/lib:${LD_LIBRARY_PATH}"
export QT_PLUGIN_PATH="${HERE}/usr/plugins"
exec "${HERE}/usr/bin/HungerYet" "$@"
EOF
chmod +x AppDir/AppRun

# 创建图标目录
mkdir -p AppDir/usr/share/icons/hicolor/256x256/apps/

# 复制图标 (根据你的实际图标路径调整)
cp /home/whx/文档/code/github/HungerYet/client/resources/icos/esu.png AppDir/usr/share/icons/hicolor/256x256/apps/HungerYet.png
# 同时在 AppDir 根目录也放置一份
cp /home/whx/文档/code/github/HungerYet/client/resources/icos/esu.png AppDir/HungerYet.png

# 修改桌面文件内容
cat > AppDir/HungerYet.desktop << EOF
[Desktop Entry]
Name=HungerYet
Exec=HungerYet
Icon=HungerYet
Type=Application
Categories=Utility;
Comment=HungerYet Application
Version=1.0
Terminal=false
EOF

# 下载AppImage工具
wget -c "https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage"
chmod +x appimagetool-x86_64.AppImage

# 创建AppImage
./appimagetool-x86_64.AppImage AppDir HungerYet.AppImage