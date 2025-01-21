#pragma once

#include "../include/Renderer.h"
#include "../include/RenderData.h"

typedef struct CollisionInfo {
  // if the scene object has multiple items
  // inside it, this identifies which one
  // was actually hit
  int collisionIndex; 

  // if the scene object has multiple groups
  // inside it, this identifies which one was 
  // hit. Individual collision indices are inside
  // this group.
  void* groupPointer;

} CollisionInfo;

class SceneObject
{
public:
  virtual void Render(Renderer* renderer, const RenderData& renderData) = 0;
  virtual void UpdateTick(double deltaTime) = 0;
  virtual bool CheckCollision(Renderer* renderer, double x, double y, double* collisionZ, CollisionInfo* info) const = 0;
  virtual void ProcessClick(CollisionInfo info) = 0;
  virtual ~SceneObject() = default;
};
