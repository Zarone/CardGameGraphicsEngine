#pragma once

#include "../include/Scene.h"
#include "../include/WindowManager.h"
#include "../example/TestCardDatabaseSingleton.h"
#include "../include/Shader.h"
#include "../include/shaders/allShaders.h"

class TestScene : public Scene
{
private:
  TestCardDatabaseSingleton* database;

  // I want to put these into a mapping system later
  Shader buttonShader = Shader(myShaders::basicVertex, myShaders::basicFragment);
public:
  TestScene(WindowManager* windowManager, TestCardDatabaseSingleton* database);
  void Swap(unsigned int sceneIndex);
};
