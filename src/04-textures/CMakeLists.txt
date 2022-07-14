add_executable(textures
  "main.cpp"
  "stb_image.h"
  "stb_image.c"
)
cmrc_add_resource_library(textures-rc NAMESPACE rc
  "fragment.glsl"
  "vertex.glsl"
  "mc_skin.png"
)
opengl_testing_target_setup(textures)