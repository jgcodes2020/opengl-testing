cmake_minimum_required(VERSION 3.21 FATAL_ERROR)
project(opengl-testing
  LANGUAGES C CXX
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

# add_library(oglc-test STATIC
#   "src/oglc-test.cpp"
# )
# cmrc_add_resource_library(oglc-test-rc NAMESPACE rc)
# target_link_libraries(oglc-test PUBLIC oglc-test-rc)
# target_compile_features(oglc-test PUBLIC cxx_std_20)
# target_include_directories(oglc-test PUBLIC "${PROJECT_SOURCE_DIR}/inc")

add_subdirectory(src/01-triangle)
add_subdirectory(src/02-uniforms)
add_subdirectory(src/03-attributes)