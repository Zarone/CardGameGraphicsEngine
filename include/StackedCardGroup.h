#include "CardGroup.h"
#include "FannedCardGroup.h"

class StackedCardGroup : public CardGroup
{
public:
  StackedCardGroup(
    Renderer* renderer,
    bool zFlipped
  );

  void MoveToGroup(int index, FannedCardGroup* to);

  void UpdateTick(double deltaTime);

  void Render(Renderer* renderer); 

  bool CheckCollision(
    Renderer* renderer, 
    double x, 
    double y, 
    double* collisionZ,
    CollisionInfo* collisioInfo
  ) const;

};
