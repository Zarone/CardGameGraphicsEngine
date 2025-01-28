#pragma once

#include "../../include/SimpleRenderObjects/SimplePlane.h"
#include "../../include/Material.h"

class SimpleButton : public SimplePlane
{
private:
  Renderer* renderer;
  glm::vec3 projectedVertices[4];
  bool pressedDown = false;
public:
  SimpleButton(
    Renderer* renderer,
    glm::mat4 transform,
    Material material
  );
  bool CheckCollision(
    Renderer* renderer, 
    double x, 
    double y, 
    double* collisionZ, 
    CollisionInfo* info
  ) const;
  ClickEvent ProcessPreClick(CollisionInfo info);
  virtual ClickEvent ProcessClick(CollisionInfo info);
  void SetupProjectedVertices(glm::mat4* transform);
  void SetTransform(glm::mat4* transform);
  void ReleaseClick();
};
