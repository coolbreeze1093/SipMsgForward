# 指定CMake的最低版本要求
cmake_minimum_required(VERSION 2.8.0)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--copy-dt-needed-entries -fPIC -std=c++11")

# 定义项目名称
project(Proxy)

# 添加包含目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 查找Boost库
find_package(Boost REQUIRED COMPONENTS system filesystem)
include_directories(${Boost_INCLUDE_DIRS})

# 添加所有源文件到一个变量中
file(GLOB SOURCES "*.cpp")

# 添加一个可执行文件
# 第一个参数是可执行文件的名称
# 后面的参数是构成这个可执行文件的源文件
add_executable(Proxy ${SOURCES})

# 链接Boost库
target_link_libraries(Proxy ${Boost_LIBRARIES})
