cmake_minimum_required(VERSION 4.2)
project(Practica_1_PAG)

set(CMAKE_CXX_STANDARD 20)

# Define variables agrupando nombres de archivo
file( GLOB MY_FILES *.cpp ) # Código fuente de tu proyecto
file( GLOB GLAD_FILES GLAD/src/*.c ) # Código fuente de GLAD

add_executable(${PROJECT_NAME} ${MY_FILES}
                               ${GLAD_FILES})

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL glfw)

target_include_directories( ${PROJECT_NAME} PUBLIC GLAD/include )

