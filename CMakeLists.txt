cmake_minimum_required(VERSION 3.31)
project(traffic_light_system)

set(CMAKE_CXX_STANDARD 20)
list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Crow)

add_executable(
        traffic_light_system
        Graph_Architecture/Edge.cpp
        Graph_Architecture/Edge.h
        Dataset/Dataset.cpp
        Dataset/Dataset.h
        Graph_Architecture/Vertex.cpp
        Graph_Architecture/Vertex.h
        Graph_Architecture/Graph.cpp
        Graph_Architecture/Graph.h
        api.cpp
        utils.h
)

target_link_libraries(traffic_light_system PRIVATE CURL::libcurl)
target_link_libraries(traffic_light_system PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(traffic_light_system PRIVATE Crow::Crow)