#pragma once

#include "../include/Scene.h"
#include "../include/WindowManager.h"
#include "../example/TestCardDatabaseSingleton.h"

class TestScene : public Scene
{
private:
  TestCardDatabaseSingleton* database;
public:
  TestScene(WindowManager* windowManager, TestCardDatabaseSingleton* database);
  void Swap(unsigned int sceneIndex);
};
