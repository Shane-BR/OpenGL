cmake_minimum_required(VERSION 4.3)
project(opengl)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL3 CONFIG REQUIRED)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} 
PRIVATE
    src/main.cpp
)

add_subdirectory(lib/glad/ EXCLUDE_FROM_ALL)

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 glad)