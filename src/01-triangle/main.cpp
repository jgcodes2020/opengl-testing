#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>
#include <numbers>
#include "oglc/handles.hpp"
#define GLFW_INCLUDE_NONE
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <GLFW/glfw3.h>

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(rc);

#include <iostream>
#include <array>
#include <cmath>

float vertices[] = {
  -0.5f, -0.5f, 0.0f,
  0.5f, -0.5f, 0.0f,
  0.5f, 0.5f, 0.0f,
  -0.5f, 0.5f, 0.0f
};
gl::GLuint indices[] = {
  0, 1, 2,
  2, 3, 0
};

gl::GLuint vbo, vao, ebo;
oglc::ShaderProgram shader;

void setup(GLFWwindow* win) {
  using namespace gl;
  glfwMakeContextCurrent(win);
  
  // Setup viewport and resize handler
  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(win, [](GLFWwindow* win, int width, int height) {
    glfwMakeContextCurrent(win);
    glViewport(0, 0, width, height);
  });
  
  // Grab shaders from resource files
  // ================================
  {
    auto fs = cmrc::rc::get_filesystem();
    shader = oglc::ShaderProgram {
      oglc::Shader::fromResource(GL_VERTEX_SHADER, fs.open("/vertex.glsl")),
      oglc::Shader::fromResource(GL_FRAGMENT_SHADER, fs.open("/fragment.glsl")),
    };
  }
  
  shader.use();
  // Construct VBO/VAO/EBO
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  glGenVertexArrays(1, &vao);
  
  // Bind VAO
  glBindVertexArray(vao);
  
  // Setup VBO data
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  // Define vertex layout: 3 floats per vertex, with no additional data
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);
  
  // Setup EBO data
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void render(GLFWwindow* win) {
  using namespace gl;
  
  glfwMakeContextCurrent(win);
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  
  // Load drawing buffers
  shader.use();
  glBindVertexArray(vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  
  // draw our elements
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void input(GLFWwindow* win) {
  if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(win, true);
  }
}

int main() {
  glfwInit();
  // Setup an OpenGL 3.3 Core context
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // Create GLFW window
  GLFWwindow* win = glfwCreateWindow(800, 600, "OpenGL Testing", nullptr, nullptr);
  if (win == nullptr) {
    glfwTerminate();
    std::cerr << "GLFW failed to create window\n";
    return 1;
  }
  glbinding::initialize(glfwGetProcAddress, false);
  
  setup(win);
  
  // event loop
  while (!glfwWindowShouldClose(win)) {
    input(win);
    render(win);
    
    // swap buffers and poll
    glfwSwapBuffers(win);
    glfwPollEvents();
  }
  
  shader.~ShaderProgram();
  glfwTerminate();
  return 0;
}