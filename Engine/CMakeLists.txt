cmake_minimum_required(VERSION 4.1)

project(HelloWorld)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(fmt CONFIG REQUIRED)

add_executable(HelloWorld  helloworld.cpp)

target_link_libraries(HelloWorld PRIVATE glad::glad glfw opengl32)