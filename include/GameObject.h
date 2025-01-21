#pragma once

#include "../include/SceneObject.h"
#include "../include/GameState.h"

class GameObject : public SceneObject
{
private:
  GameState game;
public:
  void Render(Renderer* renderer, const RenderData& renderData);
  void UpdateTick(double deltaTime);
  bool CheckCollision(
    Renderer* renderer, 
    double x, 
    double y, 
    double* collisionZ, 
    CollisionInfo* info
  ) const;
  void ProcessClick(CollisionInfo info);
};
