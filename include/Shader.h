#pragma once
#include <string>

class Shader 
{
private:
  unsigned int programID;
  static unsigned int CompileShader(unsigned int type, const std::string& source);
public:
  Shader(const std::string& vertexShader, const std::string& fragmentShader);
  void Bind();
  int GetUniformLocation(const std::string& name);
  ~Shader();
};
