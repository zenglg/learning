cmake_minimum_required(VERSION 2.8)
project(3)
find_package(OpenCV REQUIRED)
add_executable(3 3.cpp)
target_link_libraries(3 ${OpenCV_LIBS})
