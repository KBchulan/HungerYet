cmake_minimum_required(VERSION 3.10)
project(hunger_yet_rabbitmq)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 使用pkg-config查找librabbitmq
find_package(PkgConfig REQUIRED)
pkg_check_modules(RABBITMQ REQUIRED librabbitmq)

# 添加源文件
set(SOURCES
    src/rabbitmq_manager.cpp
)

# 添加头文件
set(HEADERS
    include/rabbitmq_config.h
    include/rabbitmq_manager.h
)

# 创建库
add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# 添加包含目录
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${RABBITMQ_INCLUDE_DIRS}
)

# 链接RabbitMQ库
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${RABBITMQ_LIBRARIES}
) 