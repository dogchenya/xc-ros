COPTS = [
    "-std=c++11"
]

cc_library(
    name = "robot_rpc_cpp",
    srcs = glob([
        "src/*.cpp",
        "src/**/*.cpp",
        "include/**/*.h"
    ]),
    includes = [
        "include"
    ],
    copts = COPTS,
    linkstatic = True,
)

cc_binary(
    name = "simple-test-client",
    srcs = glob([
        "simple-test/HelloClient.cpp",
    ]),
    includes = [
        "include"
    ],
    copts = COPTS,
    deps = [
        ":robot_rpc_cpp",
    ],
)

cc_binary(
    name = "simple-test-server",
    srcs = glob([
        "simple-test/HelloServer.cpp",
    ]),
    includes = [
        "include"
    ],
    copts = COPTS,
    deps = [
        ":robot_rpc_cpp",
    ],
)
