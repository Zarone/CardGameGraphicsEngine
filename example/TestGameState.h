#pragma once
#include "../include/GameState.h"
#include "../example/TestCardDatabaseSingleton.h"
#include "../include/BoundButton.h"
#include "../include/shaders/allShaders.h"

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
  ~TestGameState() {
    std::cout << "delete game state" << std::endl;
  }
  void LoadProperShader(Renderer* renderer, CardGroup* group);
  void Render(Renderer* renderer);
};
