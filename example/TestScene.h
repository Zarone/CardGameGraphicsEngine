#pragma once

#include "../src/Scene.h"
#include "../src/WindowManager.h"
#include "../example/TestCardDatabaseSingleton.h"

class TestScene : public Scene
{
private:
  TestCardDatabaseSingleton* database;
public:
  TestScene(WindowManager* windowManager, TestCardDatabaseSingleton* database);
  void Swap(unsigned int sceneIndex);
};
