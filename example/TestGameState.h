#include "../include/GameState.h"

class TestGameState : public GameState
{
private:
  CardGroup hand;
  CardGroup oppHand;
public:
  TestGameState();
  ClickEvent ProcessClick(CollisionInfo info);
  ~TestGameState() {
    std::cout << "delete game state" << std::endl;
  }
};
