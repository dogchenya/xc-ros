cmake_minimum_required(VERSION 2.8.3)
project(common)
find_package(catkin REQUIRED)
catkin_package(
  DEPENDS Boost console_bridge
  INCLUDE_DIRS include
  LIBRARIES ${PROJECT_NAME})

install(DIRECTORY include/
  DESTINATION ${CATKIN_GLOBAL_INCLUDE_DESTINATION}
  FILES_MATCHING PATTERN "*.h")
