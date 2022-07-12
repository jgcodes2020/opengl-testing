add_executable(textures
  "main.cpp"
  "stb_image.c"
  "stb_image.h"
)
cmrc_add_resource_library(textures-rc NAMESPACE rc
  "fragment.glsl"
  "vertex.glsl"
  "texture.png"
)
opengl_testing_target_setup(textures)