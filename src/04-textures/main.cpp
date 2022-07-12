#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>
#include <exception>
#include <numbers>
#include "oglc/handles.hpp"
#include "oglc/linalg.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(rc);

#include "stb_image.h"

#include <array>
#include <cmath>
#include <iostream>

struct vertex {
  // Coordinates in space
  oglc::vec3 pos;
  oglc::vec2 tc;
};

vertex vertices[] = {
  {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
  {{0.5f, -0.5f, 0.0f},  {1.0f, 0.0f}},
  {{0.5f, 0.5f, 0.0f},   {1.0f, 1.0f}},
  {{-0.5f, 0.5f, 0.0f},  {0.0f, 1.0f}}
};
gl::GLuint indices[] = {0, 1, 2, 2, 3, 0};

gl::GLuint vbo, vao, ebo, tex;
oglc::ShaderProgram shader;

void setup(GLFWwindow* win) {
  using namespace gl;
  glfwMakeContextCurrent(win);

  // Setup viewport and resize handler
  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(
    win, [](GLFWwindow* win, int width, int height) {
      glfwMakeContextCurrent(win);
      glViewport(0, 0, width, height);
    });

  // Grab shaders from resource files
  // ================================
  {
    auto fs = cmrc::rc::get_filesystem();
    shader  = oglc::ShaderProgram {
      oglc::Shader::fromResource(GL_VERTEX_SHADER, fs.open("/vertex.glsl")),
      oglc::Shader::fromResource(GL_FRAGMENT_SHADER, fs.open("/fragment.glsl")),
    };
  }

  // Grab texture from resource files
  // ================================
  {
    auto fs   = cmrc::rc::get_filesystem();
    auto file = fs.open("/texture.png");

    int width, height, channels;
    std::unique_ptr<stbi_uc[], decltype(&stbi_image_free)> data(
      stbi_load_from_memory(
        reinterpret_cast<const stbi_uc*>(file.begin()),
        file.end() - file.begin(), &width, &height, &channels, 0),
      stbi_image_free);
    if (!data.get()) {
      std::cerr << "stb_image failed to load texture\n";
      std::terminate();
    }

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(
      GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
      data.get());
    glGenerateMipmap(GL_TEXTURE_2D);
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
  // Define vertex layout
  glVertexAttribPointer(
    0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),
    reinterpret_cast<void*>(offsetof(vertex, pos)));
  glVertexAttribPointer(
    1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex),
    reinterpret_cast<void*>(offsetof(vertex, tc)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  // Setup EBO data
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void render(GLFWwindow* win) {
  using namespace gl;

  glfwMakeContextCurrent(win);
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

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
  GLFWwindow* win =
    glfwCreateWindow(800, 600, "OpenGL Testing", nullptr, nullptr);
  if (win == nullptr) {
    glfwTerminate();
    std::cerr << "GLFW failed to create window\n";
    std::terminate();
  }
  glbinding::initialize(glfwGetProcAddress, false);

  setup(win);

  // event loop
  while (!glfwWindowShouldClose(win)) {
    std::cerr << "Event loop\n";
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