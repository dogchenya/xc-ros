cmake_minimum_required(VERSION 2.8.3)
project(xcros)

# -g 生成调试信息 | -c 只编译并生成object文件 | -E只运行c预编译 | 
# -Wall 生成所有告警信息 | -w 不生成任何警告
# -W* 告警标记
# -DVALGRIND 可以检查内存问题的工具

set(CXX_FLAGS
 -g
 -DVALGRIND
 -DCHECK_PTHREAD_RETURN_VALUE
 -D_FILE_OFFSET_BITS=64
 -Wall
 -Wextra
 -Wconversion
 -Wno-unused-parameter
 -Woverloaded-virtual
 -Wpointer-arith
 -Wshadow
 -Wwrite-strings
 -march=native
 # -MMD
 -std=c++11
 )
if(CMAKE_BUILD_BITS EQUAL 32)
  list(APPEND CXX_FLAGS "-m32")
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  list(APPEND CXX_FLAGS "-Wno-null-dereference")
  list(APPEND CXX_FLAGS "-Wno-sign-conversion")
  list(APPEND CXX_FLAGS "-Wno-unused-local-typedef")
  list(APPEND CXX_FLAGS "-Wthread-safety")
  list(REMOVE_ITEM CXX_FLAGS "-rdynamic")
endif()
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

# 让编出来的二进制放bin 静态与动态放lib
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# 全局头文件目录参考
include_directories (
  ${CMAKE_CURRENT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}
  "${PROJECT_SOURCE_DIR}/common"
  "${PROJECT_SOURCE_DIR}/robotrpcpp/include"
  "${PROJECT_SOURCE_DIR}/xcroscpp_core/cpp_common/include"
  "${PROJECT_SOURCE_DIR}/xcroscpp_core/rostime/include"
  "${PROJECT_SOURCE_DIR}/xcroscpp_core/roscpp_traits/include"
  "${PROJECT_SOURCE_DIR}/xcroscpp_core/roscpp_serialization/include"
  "${PROJECT_SOURCE_DIR}/xcroscpp/include"
  "${PROJECT_SOURCE_DIR}/xcrosconsole/include"
  )

# 下一级是啥？
add_subdirectory(common)
add_subdirectory(robotrpcpp)
add_subdirectory(xcroscpp_core)
add_subdirectory(xcroscpp)


