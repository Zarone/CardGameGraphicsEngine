#include "../include/GameScene.h"
#include "../include/GameObject.h"

GameScene::GameScene(WindowManager* windowManager, CardDatabaseSingleton* database)
  :
  Scene(
    windowManager, 
    glm::vec3(0.0f, 0.0f, 10.f),
    glm::vec3(0.0f, 0.0f, -1.0f),
    database
  )
{
  this->objects.push_back(
    std::make_unique<GameObject>()
  );
}
