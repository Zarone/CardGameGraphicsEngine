#include "../include/Shader.h"
#include <GL/glew.h>
#include <iostream>

unsigned int Shader::CompileShader(unsigned int type, const std::string& source ) {
  unsigned int id = glCreateShader(type);
  const char* src = source.c_str();
  glShaderSource(id, 1, &src, NULL);
  glCompileShader(id);

  int result;
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  if (!result) {
    int length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    char* message = (char*)alloca(length * sizeof(char));
    glGetShaderInfoLog(id, length, &length, message);
    std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
    std::cout << message << std::endl;
    glDeleteShader(id);
    return 0;
  }

  return id;
}

Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader) {
  programID = glCreateProgram();
  unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
  unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

  glAttachShader(programID, vs);
  glAttachShader(programID, fs);
  glLinkProgram(programID);
  glValidateProgram(programID);

  glDeleteShader(vs);
  glDeleteShader(fs);
}

void Shader::Bind() {
  glUseProgram(this->programID);
}

int Shader::GetUniformLocation(const std::string& name) {
  return glGetUniformLocation(this->programID, name.c_str());
}

Shader::~Shader() {
  glDeleteProgram(this->programID);
}
