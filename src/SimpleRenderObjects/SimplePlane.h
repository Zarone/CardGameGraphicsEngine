#pragma once

#include "SimpleRenderObject.h"
#include "../HelperRendering/Material.h"

class SimplePlane : public SimpleRenderObject
{
private:
  static const float vertexData[];
  static const unsigned int indexData[];
  bool isClickable = false;
protected:
  int GetVertexCount();
  void FillData(float* vertexData, unsigned int* indexData);
public:
  void MakeClickable();
  void MakeUnclickable();
  bool CheckCollision(
    Renderer* renderer, 
    double x, 
    double y, 
    double* collisionZ, 
    CollisionInfo* info
  ) const;
  SimplePlane(
    glm::mat4 transform, 
    Material mat
  );
};
