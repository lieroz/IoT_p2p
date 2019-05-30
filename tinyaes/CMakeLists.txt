cmake_minimum_required(VERSION 2.8.12)
project(tinyaes C ASM)

file(GLOB SOURCES *.cc)

add_library(${PROJECT_NAME}_lib ${SOURCES})
