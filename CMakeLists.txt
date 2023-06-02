cmake_minimum_required(VERSION 3.24)
project(parser_json_csv C)

set(CMAKE_C_STANDARD 11)

add_executable(parser_json_csv main.c struct.h functions.h functions.c)
