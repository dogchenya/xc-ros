

add_executable(helloclient HelloClient.cpp)
target_link_libraries(helloclient  robotrpcpp)


add_executable(helloserver HelloServer.cpp)
target_link_libraries(helloserver  robotrpcpp)

SET(CMAKE_BUILD_TYPE "Debug")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g") 
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")