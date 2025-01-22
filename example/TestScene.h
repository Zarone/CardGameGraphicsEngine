#pragma once

#include "../include/Scene.h"
#include "../include/WindowManager.h"

enum TestSceneID {
  GAME_SCREEN = 0,
  GAME_SCREEN_2_TEST = 1,
  MAIN_MENU = 2
};

class TestScene : public Scene
{
public:
  TestScene(WindowManager* windowManager);
  void Swap(unsigned int sceneIndex);
};
