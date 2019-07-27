cmake_minimum_required(VERSION 3.0)

project(hidapi)

add_library(hidapi hidapi.h hidapi.c)
target_include_directories(hidapi PUBLIC .)
target_link_libraries(hidapi setupapi)