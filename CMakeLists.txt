cmake_minimum_required(VERSION 3.7)
project(IoT_p2p)

include_directories(lora)
add_subdirectory(lora)

include_directories(tools)
add_subdirectory(tools)

set(SOURCES
    node.cc)

add_executable(${PROJECT_NAME} main.cc ${SOURCES})
target_link_libraries(${PROJECT_NAME} lora_lib tools_lib)
