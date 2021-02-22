#cmake需要的最低版本
cmake_minimum_required(VERSION 2.8.3)
project(xcroscpp_core)

if(NOT WIN32)
    set_directory_properties(PROPERTIES COMPILE_OPTIONS "-Wall;-Wextra")
endif()

if(WIN32)
  add_definitions(-D_WINDOWS)
endif()

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
# aux_source_directory(. DIR_SRCS)

#一般是在工程主文件中添加
#向当前工程添加存放 源代码 的子目录，并指定目标二进制存放的位置。 
add_subdirectory(cpp_common)
add_subdirectory(rostime)
add_subdirectory(roscpp_serialization)