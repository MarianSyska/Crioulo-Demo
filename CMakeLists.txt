cmake_minimum_required(VERSION 3.10)
project(CriouloDemo)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)

set(SOURCES ${PROJECT_SOURCE_DIR}/src/main.cpp ${PROJECT_SOURCE_DIR}/src/glad.c ${PROJECT_SOURCE_DIR}/src/core/model_loader.cpp)

configure_file(configuration/root_directory.h.in configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)

add_subdirectory(lib/Crioulo)

add_executable(CriouloDemo ${SOURCES})
target_include_directories(CriouloDemo PRIVATE ${OPENGL_INCLUDE_DIR} include)
target_link_libraries(CriouloDemo ${OPENGL_gl_LIBRARY} ${PROJECT_SOURCE_DIR}/lib/glfw3.lib ${PROJECT_SOURCE_DIR}/lib/assimp-vc143-mtd.lib Crioulo)