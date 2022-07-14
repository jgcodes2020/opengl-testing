#version 330 core

// Passed from vertex shader.
in vec3 vcolor;
in vec2 frag_tcs;
// Output color for this fragment.
out vec4 color;

uniform sampler2D tex;

void main() {
  color = texture(tex, frag_tcs);
}