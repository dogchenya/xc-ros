# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量

aux_source_directory(. DIR_LIB_SRCS)

# 生成链接库

ADD_LIBRARY (xcroscpp SHARED ${DIR_LIB_SRCS})

ADD_LIBRARY (xcroscpp_static STATIC ${DIR_LIB_SRCS})

target_link_libraries(xcroscpp pthread rt)

install(TARGETS xcroscpp LIBRARY DESTINATION lib)