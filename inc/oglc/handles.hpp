#ifndef OGLC_HANDLES_HPP_INCLUDED
#define OGLC_HANDLES_HPP_INCLUDED

#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>
#include <glbinding/gl/gl.h>
#include <glbinding/gl/types.h>

#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <memory>

#include "cmrc/cmrc.hpp"

// OGLC: OpenGL Classes
namespace oglc {
  class Shader;
  class ShaderProgram;
  
  class Shader {
    friend class ::oglc::ShaderProgram;
  public:
    Shader() : m_handle(0) {}
    // not copyable
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    // movable
    Shader(Shader&&) = default;
    Shader& operator=(Shader&&) = default;
    
    static Shader fromString(gl::GLenum type, std::string_view code) {
      return Shader(type, code.data(), code.size());
    }
    static Shader fromResource(gl::GLenum type, cmrc::file file) {
      return Shader(type, file.begin(), file.end() - file.begin());
    }
    static Shader fromCString(gl::GLenum type, const char* code) {
      return Shader(type, code);
    }
    
    ~Shader() {
      if (m_handle == 0)
        return;
      gl::glDeleteShader(m_handle);
      m_handle = 0;
    }
    
  private:
    Shader(gl::GLenum type, const char* begin, gl::GLint len) :
      m_handle(gl::glCreateShader(type)) {
      gl::glShaderSource(m_handle, 1, &begin, &len);
      gl::glCompileShader(m_handle);
      
      int success;
      gl::glGetShaderiv(m_handle, gl::GL_COMPILE_STATUS, &success);
      if (!success) {
        gl::GLint len;
        gl::glGetShaderiv(m_handle, gl::GL_INFO_LOG_LENGTH, &len);
        std::string data(len, '\0');
        gl::glGetShaderInfoLog(m_handle, len, nullptr, data.data());
        std::cerr << "Shader compilation failed: " << data << std::endl;
        throw std::runtime_error("Shader compilation failed");
      }
      
    }
    Shader(gl::GLenum type, const char* cstr) :
      m_handle(gl::glCreateShader(type)) {
      gl::glShaderSource(m_handle, 1, &cstr, nullptr);
      gl::glCompileShader(m_handle);
      
      int success;
      gl::glGetShaderiv(m_handle, gl::GL_COMPILE_STATUS, &success);
      if (!success) {
        gl::GLint len;
        gl::glGetShaderiv(m_handle, gl::GL_INFO_LOG_LENGTH, &len);
        std::string data(len, '\0');
        gl::glGetShaderInfoLog(m_handle, len, nullptr, data.data());
        std::cerr << "Shader compilation failed: " << data << std::endl;
        throw std::runtime_error("Shader compilation failed");
      }
    }
    
    gl::GLuint m_handle;
  };
  
  class ShaderProgram {
  public:
    ShaderProgram() : m_handle(0) {}
    
    template <class... Ts>
    ShaderProgram(Ts&&... shaders) :
      m_handle(gl::glCreateProgram()){
      static_assert((std::is_same_v<Ts, ::oglc::Shader> && ...),
        "Shader program requires a list of shaders");
      
      if (((shaders.m_handle == 0) || ...)) {
        throw std::logic_error("Some shaders have not been loaded yet");
      }
      
      // bind shaders and link
      (gl::glAttachShader(m_handle, shaders.m_handle), ...);
      gl::glLinkProgram(m_handle);
      // I'm not sure why this has to be here but it does.
      use();
    }
    // not copyable
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;
    // movable
    ShaderProgram(ShaderProgram&&) = default;
    ShaderProgram& operator=(ShaderProgram&&) = default;
    
    ~ShaderProgram() {
      if (m_handle == 0)
        return;
      gl::glDeleteProgram(m_handle);
      m_handle = 0;
    }
    
    void use() {
      if (m_handle == 0)
        throw std::logic_error("Handle is not assigned to any shader");
      gl::glUseProgram(m_handle);
    }
    
    gl::GLuint handle() {
      if (m_handle == 0)
        throw std::logic_error("Handle is not assigned to any shader");
      return m_handle;
    }
    
  private:
    gl::GLuint m_handle;
  };
}
#endif