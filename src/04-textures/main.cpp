#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>
#include <exception>
#include <numbers>
#include <stdexcept>
#include "oglc/handles.hpp"
#include "oglc/linalg.hpp"
#include "stb_image.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(rc);

#include <array>
#include <cmath>
#include <iostream>

struct vtx {
  oglc::vec3 pos;
  oglc::vec3 col;
  oglc::vec2 tcs;
};

#define OGLC_VTX_ATTR(n, count, type, vtx_st, mbr)      \
  do {                                                  \
    gl::glVertexAttribPointer(                          \
      n, count, type, gl::GL_FALSE, sizeof(vtx_st),     \
      reinterpret_cast<const void*>(                    \
        static_cast<intptr_t>(offsetof(vtx_st, mbr)))); \
    gl::glEnableVertexAttribArray(n);                   \
  } while (false);

vtx vertices[] = {
  // bottom left
  {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
 // bottom right
  {{0.5f, -0.5f, 0.0f},  {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
 // top right
  {{0.5f, 0.5f, 0.0f},   {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
 // top left
  {{-0.5f, 0.5f, 0.0f},  {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}
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

  // Import resources
  // ================================
  {
    auto fs = cmrc::rc::get_filesystem();
    shader  = oglc::ShaderProgram {
      oglc::Shader::fromResource(GL_VERTEX_SHADER, fs.open("/vertex.glsl")),
      oglc::Shader::fromResource(GL_FRAGMENT_SHADER, fs.open("/fragment.glsl")),
    };

    // load a texture from STB
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    {
      auto texture = fs.open("/mc_skin.png");
      int w, h, nc;
      stbi_uc* data = stbi_load_from_memory(
        reinterpret_cast<const stbi_uc*>(texture.begin()), texture.size(), &w,
        &h, &nc, 0);
      if (!data) {
        
        throw std::runtime_error("STB failed to load image");
      }
      glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
      
      stbi_image_free(data);
    }
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
  OGLC_VTX_ATTR(0, 3, GL_FLOAT, vtx, pos);
  OGLC_VTX_ATTR(1, 3, GL_FLOAT, vtx, col);
  OGLC_VTX_ATTR(2, 2, GL_FLOAT, vtx, tcs);

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

  // Load drawing buffers
  shader.use();
  glBindTexture(GL_TEXTURE_2D, tex);
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