#cmake需要的最低版本
cmake_minimum_required(VERSION 2.8.3)
project(robotrpcpp)

if(NOT WIN32)
    set_directory_properties(PROPERTIES COMPILE_OPTIONS "-Wall;-Wextra")
endif()

if(WIN32)
  add_definitions(-D_WINDOWS)
endif()

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)

include_directories (
  ${PROJECT_SOURCE_DIR}
  "${PROJECT_SOURCE_DIR}/include"
  )

#一般是在工程主文件中添加
#向当前工程添加存放 源代码 的子目录，并指定目标二进制存放的位置。 
add_subdirectory(src lib)
add_subdirectory(simple-test)

#add_library(robotrpcpp
#  src/RobotRpcSocket.cpp
#)


SET(CMAKE_BUILD_TYPE "Debug")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g") 
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")