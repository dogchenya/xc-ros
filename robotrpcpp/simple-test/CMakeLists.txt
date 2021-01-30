

add_executable(helloclient HelloClient.cpp)
target_link_libraries(helloclient  robotrpcpp)


add_executable(helloserver HelloServer.cpp)
target_link_libraries(helloserver  robotrpcpp)

add_executable(testbase64client TestBase64Client.cpp)
target_link_libraries(testbase64client  robotrpcpp)

add_executable(testbase64server TestBase64Server.cpp)
target_link_libraries(testbase64server  robotrpcpp)