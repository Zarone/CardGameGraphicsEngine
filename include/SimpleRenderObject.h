#pragma once

#include <glm/glm.hpp>

#include "../include/VertexArray.h"
#include "../include/IndexBuffer.h"
#include "../include/Shader.h"

class SimpleRenderObject
{
private:
  VertexArray vArray;
  VertexBuffer vBuffer;
  IndexBuffer iBuffer;
  Shader shader;
public:
  SimpleRenderObject(glm::vec3 position, const std::string& shaderCode);
};
