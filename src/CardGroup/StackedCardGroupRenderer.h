#pragma once
#include "CardGroupRenderer.h"

class StackedCardGroupRenderer : public CardGroupRenderer
{
private:
  // Max number of cards rendered in the stack
  const static unsigned int RenderMax = 7;
public:
  StackedCardGroupRenderer(
    Renderer* renderer,
    glm::vec3 position, 
    float rotationX, 
    bool zFlipped
  );

  const glm::mat4 WorldSpaceToThisSpace();

  void Render(Renderer* renderer); 
  bool CheckCollision(
    Renderer* renderer, 
    double x, 
    double y, 
    double* collisionZ,
    CollisionInfo* collisioInfo
  ) const;
};
