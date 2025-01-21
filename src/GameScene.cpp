#include "../include/GameScene.h"
#include "../include/GameObject.h"

GameScene::GameScene(WindowManager* windowManager):
  Scene(
    windowManager, 
    glm::vec3(0.0f, 0.0f, 10.f),
    glm::vec3(0.0f, 0.0f, -1.0f)
  )
{
  this->objects.push_back(
    std::make_unique<GameObject>()
  );
  //this->objects.push_back(
    //std::make_unique<CardGroup>(
      //&this->textureMap,
      //glm::vec3(0.0f, -2.0f, 4.0f),
      ////glm::vec3(0.0f, -0.5f, 6.0f),
      //-20.0f, // rotateX
      //4.0f, // width
      //false,// z flipped
      //true
    //)
  //);
  //this->objects.push_back(
    //std::make_unique<CardGroup>(
      //&this->textureMap,
      //glm::vec3(0.0f, 2.0f, 1.0f),
      ////glm::vec3(0.0f, -0.5f, 6.0f),
      //40.0f, // rotateX
      //4.0f, // width
      //true,// z flipped
      //true
    //)
  //);
}
