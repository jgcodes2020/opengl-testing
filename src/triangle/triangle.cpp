#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>
#define GLFW_INCLUDE_NONE
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <GLFW/glfw3.h>

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(rc);

#include <iostream>
#include <array>

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

gl::GLuint vbo, vao, ebo, shp;

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
    GLuint vertex_shader, fragment_shader;
    {
      auto file = fs.open("/vertex.glsl");
      vertex_shader = glCreateShader(GL_VERTEX_SHADER);
      const char* data_begin = file.begin();
      const GLsizei data_size = file.end() - file.begin();
      glShaderSource(vertex_shader, 1, &data_begin, &data_size);
      glCompileShader(vertex_shader);
    }
    {
      auto file = fs.open("/fragment.glsl");
      fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
      const char* data_begin = file.begin();
      const GLsizei data_size = file.end() - file.begin();
      glShaderSource(fragment_shader, 1, &data_begin, &data_size);
      glCompileShader(fragment_shader);
    }
    shp = glCreateProgram();
    glAttachShader(shp, vertex_shader);
    glAttachShader(shp, fragment_shader);
    glLinkProgram(shp);
    
    glUseProgram(shp);
    
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
  }
  
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
  
  // do our actual rendering
  glUseProgram(shp);
  glBindVertexArray(vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
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
  
  glfwTerminate();
  return 0;
}