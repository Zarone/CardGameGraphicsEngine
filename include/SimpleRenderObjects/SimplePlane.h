#pragma once

#include "../../include/SimpleRenderObjects/SimpleRenderObject.h"
#include "../../include/Material.h"

class SimplePlane : public SimpleRenderObject
{
private:
  static const float vertexData[];
  static const unsigned int indexData[];
protected:
  int GetVertexCount();
  void FillData(float* vertexData, unsigned int* indexData);
public:
  SimplePlane(
    glm::mat4 transform, 
    Material mat
  );
};
