cmake_minimum_required(VERSION 3.7)
project(IoT_p2p)

include_directories(lora)
add_subdirectory(lora)

add_executable(ping ping.cc)
target_link_libraries(ping lora_lib)

add_executable(pong pong.cc)
target_link_libraries(pong lora_lib)
