cmake_minimum_required(VERSION 3.12)
project(CourseProjectMain)

set(CMAKE_CXX_STANDARD 17)

add_executable(CourseProjectMain main.cpp TextFileSource.h Record.h TupleUtil.h DataSourceStrategy.h Iterator.h Tree.h)