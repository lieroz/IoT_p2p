cmake_minimum_required(VERSION 3.7)
project(IoT_p2p)

add_subdirectory(lora)

add_executable(server server.cpp)
target_link_libraries(server lora_lib)

add_executable(client client.cpp)
target_link_libraries(client lora_lib)

