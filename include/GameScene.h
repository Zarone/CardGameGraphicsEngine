#pragma once

#include "../include/Scene.h"
#include "../include/CardDatabaseSingleton.h"

class GameScene : public Scene
{
public:
  GameScene(WindowManager* windowManager, CardDatabaseSingleton* database);
};
