cmake_minimum_required(VERSION 2.6)
project(fakeshellcpp)

add_executable(fakeshellcpp main.cpp)

install(TARGETS fakeshellcpp RUNTIME DESTINATION bin)
