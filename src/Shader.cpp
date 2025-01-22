#include <GL/glew.h>
#include <iostream>
#include "../include/Shader.h"
#include "../include/ErrorHandling.h"

unsigned int Shader::CompileShader(unsigned int type, const std::string& source ) {
  GLCall(unsigned int id = glCreateShader(type));
  const char* src = source.c_str();
  GLCall(glShaderSource(id, 1, &src, NULL));
  GLCall(glCompileShader(id));

  int result;
  GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
  if (!result) {
    int length;
    GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
    char* message = (char*)alloca(length * sizeof(char));
    GLCall(glGetShaderInfoLog(id, length, &length, message));
    std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
    std::cout << message << std::endl;
    GLCall(glDeleteShader(id));
    return 0;
  }

  return id;
}

Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader) {
  GLCall(this->programID = glCreateProgram());
  unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
  unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

  GLCall(glAttachShader(programID, vs));
  GLCall(glAttachShader(programID, fs));
  GLCall(glLinkProgram(programID));
  GLCall(glValidateProgram(programID));

  GLCall(glDeleteShader(vs));
  GLCall(glDeleteShader(fs));
}

void Shader::Bind() {
  GLCall(glUseProgram(this->programID));
}

int Shader::GetUniformLocation(const std::string& name) {
  this->Bind();
  GLCall(int x = glGetUniformLocation(this->programID, name.c_str()));

  return x;
}

void Shader::SetUniform4fv(const std::string& name, bool transpose, GLfloat* pointer) {
  GLint thisID = this->GetUniformLocation(name);
  GLCall(glUniformMatrix4fv(thisID, 1, transpose, pointer));
}

void Shader::SetUniform1iv(const std::string& name, unsigned int count, GLint* data) {
  GLint thisID = this->GetUniformLocation(name);
  GLCall(glUniform1iv(thisID, count, data));
}

void Shader::SetUniform1i(const std::string& name, GLuint val) {
  GLint thisID = this->GetUniformLocation(name);
  GLCall(glUniform1i(thisID, val));
}

void Shader::SetUniform4f(const std::string& name, glm::vec4& vec) {
  GLint thisID = this->GetUniformLocation(name);
  GLCall(glUniform4f(thisID, vec.x, vec.y, vec.z, vec.w));
}

Shader::~Shader() {
  GLCall(glDeleteProgram(this->programID));
}
