#pragma once
#include "../include/GameState.h"
#include "../example/TestCardDatabaseSingleton.h"
#include "../include/BoundButton.h"

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
  BoundButton passTurn;
  TestCardDatabaseSingleton* database;
  void test();
public:
  TestGameState(Renderer* renderer, TestCardDatabaseSingleton* database);
  ClickEvent ProcessClick(CollisionInfo info);
  ClickEvent ProcessPreClick(CollisionInfo info);
  void ReleaseClick();
  ~TestGameState() {
    std::cout << "delete game state" << std::endl;
  }
};
