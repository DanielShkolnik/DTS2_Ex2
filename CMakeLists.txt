cmake_minimum_required(VERSION 3.16)
project(DTS2_Ex2)

set(CMAKE_CXX_STANDARD 11)

add_executable(DTS2_Ex2 artist.h avl.h library2.h library2.cpp main2_debug.cpp musicManager.h node.h song.h keyBestHitsTree.h keyPopularityID.h hashTable.h)
