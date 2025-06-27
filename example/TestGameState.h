#pragma once
#include "TestGameplayManager.h"
#include "../src/Scene/GameState.h"
#include "../example/TestCardDatabaseSingleton.h"
#include "../src/SimpleRenderObjects/BoundButton.h"
#include "../src/shaders/allShaders.h"
#include "../src/CardGroup/FannedCardGroup.h"
#include "../src/CardGroup/ExpandableStackCardGroup.h"
#include "../src/CardGroup/TemporaryCardGroup.h"
#include "../src/Scene/CommandPalette.h"
#include "../src/SimpleRenderObjects/TextBox.h"
#include <atomic>

class TestGameState : public GameState
{
private:
  FannedCardGroup hand;
  FannedCardGroup reserve;
  FannedCardGroup battlefield;
  FannedCardGroup specials;
  ExpandableStackCardGroup deck;
  ExpandableStackCardGroup discardPile;
  FannedCardGroup oppHand;
  FannedCardGroup oppReserve;
  FannedCardGroup oppBatlefield;
  FannedCardGroup oppSpecials;
  ExpandableStackCardGroup oppDeck;
  ExpandableStackCardGroup oppDiscardPile;
  TemporaryCardGroup tempPile;
  BoundButton passTurn;
  CommandPalette palette;
  TestCardDatabaseSingleton* database;
  TestGameplayManager gameplayManager;
  std::atomic<bool> setupComplete{false};
  std::thread setupThread;
  TextBox* setupTextBox = nullptr;
  std::atomic<bool> stopSetupThread{false};

  void EndTurnButtonPress();
  void LoadCommandPalette();
  void HandleUpdate(const UpdateInfo& update);
  void ProcessAction(const GameAction& action);
  void ProcessPendingMessages();
  void Setup(std::vector<unsigned int> deck);
public:
  TestGameState(Renderer* renderer, TestCardDatabaseSingleton* database);
  ClickEvent ProcessClick(CollisionInfo info);
  void ProcessScroll(CollisionInfo info, double yOffset);
  ~TestGameState();
  void LoadProperShader(Renderer* renderer, CardGroup* group);
  void Render(Renderer* renderer);
  void UpdateTick(double deltaTime);
};
