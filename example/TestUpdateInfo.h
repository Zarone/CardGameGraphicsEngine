#pragma once

#include "TestGameplayPiles.h"
#include "TestGameMode.h"
#include <vector>

struct CardMovement {
  unsigned int cardID; // unspecified unless from opponent's hand or deck
  unsigned int gameID;
  Pile from;
  Pile to;
};

struct UpdateInfo {
  std::vector<CardMovement> movements;
  GameMode phase;
  Pile openView;
  std::vector<unsigned int> openViewCards;
  std::vector<unsigned int> selectableCards;
  bool selectedCardsChanged;
  bool selectableCardsChanged;
  int selectionMin;
  int selectionMax;
};

