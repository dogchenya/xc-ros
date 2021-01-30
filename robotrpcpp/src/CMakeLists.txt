# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量

aux_source_directory(. DIR_LIB_SRCS)

# 生成链接库

ADD_LIBRARY (robotrpcpp SHARED ${DIR_LIB_SRCS})

ADD_LIBRARY (robotrpcpp_static STATIC ${DIR_LIB_SRCS})

target_link_libraries(robotrpcpp pthread rt)

# 按照一般的习惯，静态库名字跟动态库名字应该是一致的，只是扩展名不同；
# 即：静态库名为 libhello.a； 动态库名为libhello.so ；
# 所以，希望 "hello_static" 在输出时，不是"hello_static"，而是以"hello"的名字显示，故设置如下：

# SET_TARGET_PROPERTIES(robotrpcpp_static PROPERTIES OUTPUT_NAME "robotrpcpp")

# SET_TARGET_PROPERTIES (robotrpcpp_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

# SET_TARGET_PROPERTIES (robotrpcpp PROPERTIES CLEAN_DIRECT_OUTPUT 1)

# GET_TARGET_PROPERTY(OUTPUT_VALUE robotrpcpp_static OUTPUT_NAME)

# MESSAGE(STATUS “This is the hello_static OUTPUT_NAME:”${OUTPUT_VALUE})

# 按照规则，动态库是应该包含一个版本号的，
# VERSION指代动态库版本，SOVERSION指代API版本。
# SET_TARGET_PROPERTIES (robotrpcpp PROPERTIES VERSION 1.2 SOVERSION 1)

# ARCHIVE 特指静态库，LIBRARY 特指动态库，RUNTIME
# 特指可执行目标二进制。
# INSTALL (TARGETS robotrpcpp robotrpcpp_static 
#         LIBRARY DESTINATION lib
#         ARCHIVE DESTINATION lib)
# INSTALL (FILES hello.h DESTINATION include/hello)

# install(TARGETS robotrpcpp DESTINATION lib)
