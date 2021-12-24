cmake_minimum_required(VERSION 3.10)
project(Simulazione-Transazioni C)

set(CMAKE_C_STANDARD 89)

set(SOURCE_FILES *.c *.h)

set(CMAKE_INCLUDE_PATH /include)

add_executable(Simulazione-Transazioni main.c)