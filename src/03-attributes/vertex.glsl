#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 col;

// Color of the current vertex.
out vec3 vcolor;

void main() {
  gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
  vcolor = col;
}