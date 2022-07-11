add_executable(uniforms
  "main.cpp"
)
cmrc_add_resource_library(uniforms-rc NAMESPACE rc
  "vertex.glsl"
  "fragment.glsl"
)
opengl_testing_target_setup(uniforms)