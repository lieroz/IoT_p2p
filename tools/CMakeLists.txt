cmake_minimum_required(VERSION 3.7)
project(tools)

find_library(crypto_lib crypto++)

file(GLOB SOURCES *.cc)

add_library(${PROJECT_NAME}_lib ${SOURCES})
target_link_libraries(${PROJECT_NAME}_lib ${crypto_lib})
