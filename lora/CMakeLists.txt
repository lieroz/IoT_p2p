cmake_minimum_required(VERSION 3.7)
project(lora)

find_library(pthread_lib pthread)
find_library(pigpio_lib pigpio)

file(GLOB SOURCES *.cc)

add_library(${PROJECT_NAME}_lib ${SOURCES})
target_link_libraries(${PROJECT_NAME}_lib ${pthread_lib} ${pigpio_lib})
