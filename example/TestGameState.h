#pragma once
#include "../include/GameState.h"
#include "../example/TestCardDatabaseSingleton.h"

class TestGameState : public GameState
{
private:
  CardGroup hand;
  CardGroup oppHand;
  CardGroup reserve;
  CardGroup battlefield;
  CardGroup specials;
  CardGroup oppReserve;
  CardGroup oppBatlefield;
  CardGroup oppSpecials;
  TestCardDatabaseSingleton* database;
public:
  TestGameState(TestCardDatabaseSingleton* database);
  ClickEvent ProcessClick(CollisionInfo info);
  ClickEvent ProcessPreClick(CollisionInfo info);
  void ReleaseClick();
  ~TestGameState() {
    std::cout << "delete game state" << std::endl;
  }
};
