cmake_minimum_required(VERSION 3.1)
project(chesspadNEW)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    chesspadNEW/chesspad.h
    chesspadNEW/chesspadORI.cpp
    chesspadNEW/main.cpp
    chesspadNEW/openglSupport.cpp
    chesspadNEW/openglSupport.h)

add_executable(chesspadNEW ${SOURCE_FILES})