#pragma once

#include "../include/SimpleRenderObject.h"

class SimplePlane : public SimpleRenderObject
{
private:
  static const float vertexData[];
  static const unsigned int indexData[];
public:
  SimplePlane(const std::string& vertShader, const std::string& fragShader, glm::vec4 color);
  int GetVertexCount();
  void FillData(float* vertexData, unsigned int* indexData);
};
