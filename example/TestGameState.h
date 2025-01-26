#pragma once
#include "../include/GameState.h"
#include "../example/TestCardDatabaseSingleton.h"
#include "../include/BoundButton.h"
#include "../include/shaders/allShaders.h"
#include "../include/FannedCardGroup.h"
#include "../include/StackedCardGroup.h"

class TestGameState : public GameState
{
private:
  FannedCardGroup hand;
  FannedCardGroup oppHand;
  FannedCardGroup reserve;
  FannedCardGroup battlefield;
  FannedCardGroup specials;
  FannedCardGroup oppReserve;
  FannedCardGroup oppBatlefield;
  FannedCardGroup oppSpecials;
  StackedCardGroup deck;
  BoundButton passTurn;
  TestCardDatabaseSingleton* database;

  void test();
public:
  TestGameState(Renderer* renderer, TestCardDatabaseSingleton* database);
  ClickEvent ProcessClick(CollisionInfo info);
  ~TestGameState() {
    std::cout << "delete game state" << std::endl;
  }
  void LoadProperShader(Renderer* renderer, CardGroup* group);
  void Render(Renderer* renderer);
};
