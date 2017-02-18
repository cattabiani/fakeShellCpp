cmake_minimum_required(VERSION 2.6)
project(fakeshellcpp)

add_executable(fakeshellcpp main.cpp fileSystem.cpp parser.cpp)

install(TARGETS fakeshellcpp RUNTIME DESTINATION bin)
