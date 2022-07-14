#version 330 core

// Passed from vertex shader.
in vec3 vcolor;
in vec2 frag_tcs;
// Output color for this fragment.
out vec4 color;

uniform sampler2D tex;

void main() {
  vec3 bg = vec3(0.2f, 0.3f, 0.3f);
  vec4 sm = texture(tex, frag_tcs);
  
  color = vec4(mix(bg, sm.xyz, sm.a), 1.0f);
}