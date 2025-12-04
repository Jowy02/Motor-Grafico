cmake_minimum_required(VERSION 4.1)

project(SuperNova_Engine)

find_package(SDL3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(DevIL REQUIRED)
find_package(OpenGL REQUIRED)


file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")

add_executable(SuperNova_Engine ${SOURCES})

target_link_libraries(SuperNova_Engine PRIVATE SDL3::SDL3 glad::glad glm::glm imgui::imgui assimp::assimp DevIL::IL opengl32)
