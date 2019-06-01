cmake_minimum_required(VERSION 3.7)
project(crypto)

file(GLOB SOURCES *.cc)

add_library(${PROJECT_NAME}_lib ${SOURCES})
