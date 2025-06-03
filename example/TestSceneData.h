#pragma once
#include "../src/Scene/SceneData.h"
#include "../src/Card/Card.h"

enum TestSceneID {
  UNINITIALIZED = 0,
  GAME_SCREEN,
  MAIN_MENU 
};

struct GameInitData : public SceneData
{
public:
  Card hand[5];
  Card oppHand[5];
};
