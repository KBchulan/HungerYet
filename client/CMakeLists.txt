cmake_minimum_required(VERSION 3.2)

project(HungerYet VERSION 1.0 LANGUAGES CXX)
add_compile_options(-Wno-deprecated-declarations)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt设置
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 设置Qt5的路径
set(Qt5_DIR "/home/whx/qtware/5.14.2/gcc_64/lib/cmake/Qt5")
set(CMAKE_PREFIX_PATH "/home/whx/qtware/5.14.2/gcc_64")
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR})

# 查找Qt5包
find_package(Qt5 REQUIRED COMPONENTS 
    Core
    Gui
    Widgets
    Network
)
find_package(OpenCV REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(CURL REQUIRED)

# 源文件
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# 头文件
file(GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# UI文件
file(GLOB FORMS ${CMAKE_CURRENT_SOURCE_DIR}/*.ui)

# 资源文件
set(RESOURCES
    resources.qrc
)

# 创建可执行文件
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${FORMS}
    ${RESOURCES}
)

# 链接Qt库
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
    jsoncpp_lib
    CURL::libcurl
    ${OpenCV_LIBS}
)

# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 添加安装规则
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# 安装依赖的Qt库
install(CODE "
    include(BundleUtilities)
    fixup_bundle(\${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME} \"\" \"\")
")

# 安装资源文件
install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources
    DESTINATION share/${PROJECT_NAME}
)