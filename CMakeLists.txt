cmake_minimum_required(VERSION 3.24)
project(image_merge C)

set(CMAKE_C_STANDARD 99)

add_executable(image_merge main.c image_utils.c image_utils.h)
