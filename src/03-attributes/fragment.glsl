#version 330 core

// Passed from vertex shader.
in vec3 vcolor;
// Output color for this fragment.
out vec4 color;

void main() {
  color = vec4(vcolor, 1.0);
}