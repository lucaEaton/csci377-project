cmake_minimum_required(VERSION 3.31)
project(traffic_light_system)

set(CMAKE_CXX_STANDARD 20)

add_executable(
        traffic_light_system
        main.cpp
        Graph_Architecture/Edge.cpp
        Graph_Architecture/Edge.h
        Graph_Architecture/Node.cpp
        Graph_Architecture/Node.h
        Dataset/Dataset.cpp
        Dataset/Dataset.h)

find_package(CURL REQUIRED)
target_link_libraries(traffic_light_system PRIVATE CURL::libcurl)