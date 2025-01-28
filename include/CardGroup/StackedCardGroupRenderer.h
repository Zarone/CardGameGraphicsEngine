#include "CardGroupRenderer.h"

class StackedCardGroupRenderer : public CardGroupRenderer
{
private:
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
