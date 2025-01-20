#pragma once

#include <glm/glm.hpp>

#include "../include/VertexArray.h"
#include "../include/IndexBuffer.h"
#include "../include/Shader.h"
#include "../include/Renderer.h"

class SimpleRenderObject
{
private:
  const int vertexSize = 5; // 3 for position, 2 for texture
  VertexArray vArray;
  VertexBuffer vBuffer;
  IndexBuffer iBuffer;
  Shader shader;
  int triangleCount;
  glm::vec4 color;
public:
  SimpleRenderObject(
    const std::string& vertexShader, 
    const std::string& fragmentShader,
    glm::vec4 color
  );
  virtual int GetVertexCount() = 0;
  virtual void FillData(float* vertexData, unsigned int* indexData) = 0;
  void LoadIntoGPU();
  void Render(glm::mat4& transform, Renderer& renderer);
};
