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

find_package(bash-completion)
if(BASH_COMPLETION_FOUND)
  message(STATUS "Using bash completion dir ${BASH_COMPLETION_COMPLETIONSDIR}")
else()
  set (BASH_COMPLETION_COMPLETIONSDIR "/etc/bash_completion.d")
  message (STATUS "Using fallback bash completion dir ${BASH_COMPLETION_COMPLETIONSDIR}")
endif()

install(FILES ffs_completion
        DESTINATION ${BASH_COMPLETION_COMPLETIONSDIR}
        RENAME ffs)

SET(CPACK_GENERATOR "DEB")
SET(CPACK_PACKAGE_CONTACT "胡玮文 <huww98@outlook.com>")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "基于文件的模拟文件系统")
SET(CPACK_PACKAGE_DESCRIPTION "基于文件的模拟文件系统。\n")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.3.1-6), libgcc1 (>= 1:3.4.2-12)")
INCLUDE(CPack)
