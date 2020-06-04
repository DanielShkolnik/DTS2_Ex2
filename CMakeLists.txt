cmake_minimum_required(VERSION 3.13)
project(DTS2_Ex1)

set(CMAKE_CXX_STANDARD 11)

add_executable(DTS2_Ex1 avl.h node.h song.h artist.h disc.h musicManager.h library1.cpp main_debug.cpp)