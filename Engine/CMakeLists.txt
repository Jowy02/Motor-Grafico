cmake_minimum_required(VERSION 4.1)

project(HelloWorld)

find_package(SDL2 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")

add_executable(HelloWorld ${SOURCES})

target_link_libraries(HelloWorld PRIVATE SDL2::SDL2 glad::glad glm::glm opengl32)