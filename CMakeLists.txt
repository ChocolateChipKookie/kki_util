cmake_minimum_required(VERSION 3.17)
project(kki_util)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(kki_util main.cpp ../../../Faks/semestar_9/NENR/Zadace/zad_3/string.h util.h random.h string.h)