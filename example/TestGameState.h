#pragma once
#include "TestGameplayManager.h"
#include "../src/Scene/GameState.h"
#include "../example/TestCardDatabaseSingleton.h"
#include "../src/SimpleRenderObjects/BoundButton.h"
#include "../src/shaders/allShaders.h"
#include "../src/CardGroup/FannedCardGroup.h"
#include "../src/CardGroup/ExpandableStackCardGroup.h"

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
  TestGameplayManager gameplayManager;

  void EndTurnButtonPress();
public:
  TestGameState(Renderer* renderer, TestCardDatabaseSingleton* database);
  ClickEvent ProcessClick(CollisionInfo info);
  void ProcessScroll(CollisionInfo info, double yOffset);
  ~TestGameState() {
    std::cout << "delete game state" << std::endl;
  }
  void LoadProperShader(Renderer* renderer, CardGroup* group);
  void Render(Renderer* renderer);
};
