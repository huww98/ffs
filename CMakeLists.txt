cmake_minimum_required(VERSION 3.5)
project(ffs)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

add_executable(ffs main.cpp user.cpp help.cpp)

install(TARGETS ffs
        RUNTIME DESTINATION bin
        CONFIGURATIONS Release)
