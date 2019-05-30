cmake_minimum_required(VERSION 3.7)
project(IoT_p2p)

include_directories(lora)
add_subdirectory(lora)

include_directories(tinyaes)
add_subdirectory(tinyaes)

add_executable(${PROJECT_NAME} node.cc)
target_link_libraries(${PROJECT_NAME} lora_lib tinyaes_lib)
