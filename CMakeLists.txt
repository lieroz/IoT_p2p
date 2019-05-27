cmake_minimum_required(VERSION 3.7)
project(IoT_p2p)

include_directories(lora)
add_subdirectory(lora)

add_executable(${PROJECT_NAME}_ping ping.cc)
target_link_libraries(${PROJECT_NAME}_ping lora_lib)

add_executable(${PROJECT_NAME}_pong pong.cc)
target_link_libraries(${PROJECT_NAME}_pong lora_lib)
