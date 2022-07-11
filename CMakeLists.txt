cmake_minimum_required(VERSION 3.21 FATAL_ERROR)
project(opengl-testing
  LANGUAGES CXX
  DESCRIPTION "Just OpenGL testing."
)

include(cmake/CMakeRC.cmake)

find_package(glfw3 REQUIRED)
find_package(glbinding REQUIRED)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/out")

macro(opengl_testing_target_setup name)
  target_link_libraries(${name} PUBLIC 
    glbinding::glbinding glfw ${name}-rc
  )
  target_compile_features(${name} PUBLIC cxx_std_20)
  target_include_directories(${name} PUBLIC "${PROJECT_SOURCE_DIR}/inc")
endmacro()

add_subdirectory(src/triangle)
add_subdirectory(src/uniforms)