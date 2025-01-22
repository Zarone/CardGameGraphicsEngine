#pragma once

#include "../include/Scene.h"
#include "../include/WindowManager.h"

class TestScene : public Scene
{
public:
  TestScene(WindowManager* windowManager, CardDatabaseSingleton* database);
  void Swap(unsigned int sceneIndex);
};
