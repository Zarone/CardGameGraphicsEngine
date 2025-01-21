#pragma once

#include "../include/Renderer.h"
#include "../include/RenderData.h"
#include "../include/CardGroup.h"

class GameState
{
private:
  //std::vector<Action> actionStack;
  CardGroup hand;
  CardGroup oppHand;
public:
  GameState();
  void Render(Renderer* renderer, const RenderData& renderData);
  void UpdateTick(double deltaTime);
  bool CheckCollision(
    Renderer* renderer, 
    double x, 
    double y, 
    double* collisionZ, 
    CollisionInfo* info
  ) const;
};
