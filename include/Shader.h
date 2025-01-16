#pragma once
#include <string>
#include <GL/glew.h>

class Shader 
{
private:
  unsigned int programID;
  static unsigned int CompileShader(unsigned int type, const std::string& source);
public:
  Shader(const std::string& vertexShader, const std::string& fragmentShader);
  void Bind();
  int GetUniformLocation(const std::string& name);
  void SetUniform4fv(const std::string& name, bool transpose, GLfloat* pointer);
  ~Shader();
};
