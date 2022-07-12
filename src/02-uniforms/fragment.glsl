#version 330 core
#extension GL_ARB_explicit_uniform_location : require

out vec4 color;

layout(location = 2) uniform vec4 drawColor;

void main() {
  color = drawColor;
}