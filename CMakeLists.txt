cmake_minimum_required(VERSION 3.20)
project(Practica_1_PAG)

set(CMAKE_CXX_STANDARD 20)

# Define variables agrupando nombres de archivo
file( GLOB GLAD_FILES GLAD/src/*.c ) # Código fuente de GLAD
file( GLOB MY_FILES *.cpp ) # Código fuente de tu proyecto
file( GLOB IMGUI_FILES imgui/src/*.cpp)

# Comentario para probar si funciona commit y push

add_executable(${PROJECT_NAME} ${MY_FILES}
                               ${GLAD_FILES}
                               ${IMGUI_FILES})

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(assimp REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL glfw assimp::assimp)

target_include_directories( ${PROJECT_NAME} PUBLIC GLAD/include
                                                imgui/include
                                                ${CMAKE_SOURCE_DIR})

