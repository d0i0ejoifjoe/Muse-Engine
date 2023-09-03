cmake_minimum_required(VERSION 3.19)
project(sandbox)

add_executable(${PROJECT_NAME} "main.cpp")

target_link_libraries(${PROJECT_NAME} PRIVATE muse)
target_include_directories(${PROJECT_NAME} PRIVATE "./../../")