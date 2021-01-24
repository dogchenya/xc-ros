#cmake需要的最低版本
cmake_minimum_required(VERSION 2.8.3)
project(robotrpcpp)

if(NOT WIN32)
    set_directory_properties(PROPERTIES COMPILE_OPTIONS "-Wall;-Wextra")
endif()

if(WIN32)
  add_definitions(-D_WINDOWS)
endif()

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)

include_directories (
  ${PROJECT_SOURCE_DIR}
  "${PROJECT_SOURCE_DIR}/include"
  )
add_subdirectory(src)

#add_library(robotrpcpp
#  src/RobotRpcSocket.cpp
#)

add_executable(main main.cpp)
#增加链接库
target_link_libraries(main robotrpcpp)

add_executable(server server.cpp)
#增加链接库
target_link_libraries(server robotrpcpp)

add_executable(client client.cpp)
#增加链接库
target_link_libraries(client robotrpcpp)

add_executable(helloclient
    ${PROJECT_SOURCE_DIR}/simple-test/HelloClient.cpp)
#增加链接库
target_link_libraries(helloclient  robotrpcpp)

add_executable(helloserver
    ${PROJECT_SOURCE_DIR}/simple-test/HelloServer.cpp)
#增加链接库
target_link_libraries(helloserver  robotrpcpp)



SET(CMAKE_BUILD_TYPE "Debug")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g") 
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")