#pragma once
#include "../include/SceneData.h"
#include "../include/Card.h"

enum TestSceneID {
  UNINITIALIZED = 0,
  GAME_SCREEN = 1,
  GAME_SCREEN_2_TEST = 2,
  MAIN_MENU = 3
};

struct GameInitData : public SceneData
{
public:
  Card hand[5];
  Card oppHand[5];
};
