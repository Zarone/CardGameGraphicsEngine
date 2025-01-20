#pragma once
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Shader 
{
private:
  static unsigned int CompileShader(unsigned int type, const std::string& source);
  unsigned int programID;
public:
  Shader();
  Shader(const std::string& vertexShader, const std::string& fragmentShader);
  void Bind();
  int GetUniformLocation(const std::string& name);
  void SetUniform4fv(const std::string& name, bool transpose, GLfloat* pointer);
  void SetUniform4f(const std::string& name, glm::vec4& vec);
  void SetUniform1iv(const std::string& name, unsigned int count, GLint* data);
  void SetUniform1i(const std::string& name, GLuint val);
  ~Shader();
};
