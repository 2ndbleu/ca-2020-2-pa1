cmake_minimum_required(VERSION 3.13)
project(clion_huffman C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(clion_huffman
        pa1-main.c
        pa1-test.c
        pa1-test.h
        pa1.c)
