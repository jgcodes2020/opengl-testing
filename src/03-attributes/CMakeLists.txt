add_executable(attributes
  "main.cpp"
)
cmrc_add_resource_library(attributes-rc NAMESPACE rc
  "fragment.glsl"
  "vertex.glsl"
)
opengl_testing_target_setup(attributes)