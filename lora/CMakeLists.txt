cmake_minimum_required(VERSION 3.7)
project(lora)

find_library(bcm2835_lib bcm2835)

file(GLOB SOURCES *.cpp)

add_library(${PROJECT_NAME}_lib ${SOURCES})
target_link_libraries(${PROJECT_NAME}_lib ${bcm2835_lib})
