add_executable(triangle
  "triangle.cpp"
)
cmrc_add_resource_library(triangle-rc NAMESPACE rc
  "fragment.glsl"
  "vertex.glsl"
)
opengl_testing_target_setup(triangle)