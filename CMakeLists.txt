cmake_minimum_required(VERSION 3.10)
project(ffs CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

file(GLOB SRC_FILES "*.cpp")

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} stdc++fs)

install(TARGETS ffs
        RUNTIME DESTINATION bin)
