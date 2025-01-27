#pragma once
#include "../include/GameState.h"
#include "../example/TestCardDatabaseSingleton.h"
#include "../include/BoundButton.h"
#include "../include/shaders/allShaders.h"
#include "../include/FannedCardGroup.h"
#include "../include/ExpandableStackCardGroup.h"

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
  ExpandableStackCardGroup deck;
  ExpandableStackCardGroup discardPile;
  BoundButton passTurn;
  TestCardDatabaseSingleton* database;

  void EndTurnButtonPress();
public:
  TestGameState(Renderer* renderer, TestCardDatabaseSingleton* database);
  ClickEvent ProcessClick(CollisionInfo info);
  ~TestGameState() {
    std::cout << "delete game state" << std::endl;
  }
  void LoadProperShader(Renderer* renderer, CardGroup* group);
  void Render(Renderer* renderer);
};
