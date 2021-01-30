# aux_source_directory(*.proto PROTOFILES)
file(GLOB PROTOFILES *.proto)

INCLUDE(FindProtobuf)
FIND_PACKAGE(Protobuf REQUIRED)
INCLUDE_DIRECTORIES(${PROTOBUF_INCLUDE_DIR})
PROTOBUF_GENERATE_CPP(PROTO_SRC PROTO_HEADER ${PROTOFILES})
ADD_LIBRARY(proto ${PROTO_HEADER} ${PROTO_SRC})

add_executable(prototest protoTest.cpp)
target_link_libraries(prototest  proto ${PROTOBUF_LIBRARY})