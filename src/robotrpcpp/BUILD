COPTS = [
    "-std=c++11"
]

cc_library(
    name = "robotrpcpp_lib",
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
    visibility = [
        "//xcroscpp:__pkg__",
        ]
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
        ":robotrpcpp_lib",
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
        ":robotrpcpp_lib",
    ],
)
