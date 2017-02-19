cmake_minimum_required(VERSION 2.6)
project(fakeshellcpp)

add_executable(fakeShell main.cpp fileSystem.cpp parser.cpp)

add_executable(fakeShellTest mainTests.cpp fileSystem.cpp parser.cpp)