# 命令不区分大小写
# 变量区分大小写
project(skiplist)
cmake_minimum_required(VERSION 2.6)
aux_source_directory(${PROJECT_SOURCE_DIR}/src dir_src)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
add_executable(skiplist ${dir_src})

install (TARGETS skiplist DESTINATION bin)



