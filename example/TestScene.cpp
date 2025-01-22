#include <iostream>
#include "./TestScene.h"
#include "./TestGameState.h"
#include "./TestSceneData.h"
#include "../include/SimpleButton.h"

TestScene::TestScene(WindowManager* windowManager, CardDatabaseSingleton* database):
  Scene(windowManager, database)
{
}

void TestScene::Swap(unsigned int sceneIndex) {
  std::cout << "swap request to scene index " << sceneIndex << std::endl;

  switch ((TestSceneID)sceneIndex) {
    case TestSceneID::GAME_SCREEN: {
      std::cout << "swap to game screen" << std::endl;

      this->SetupCamera(
        glm::vec3(0.0f, 0.0f, 10.f),
        glm::vec3(0.0f, 0.0f, -1.0f)
      );
      this->AddObject<TestGameState>();

      break;
    }
    case TestSceneID::GAME_SCREEN_2_TEST: {
      std::cout << "swap to game screen 2" << std::endl;

      // get current information from scene

      this->Reset();
      this->AddObject<TestGameState>();
      break;
    }
    case TestSceneID::MAIN_MENU: {
      std::cout << "swap to main menu" << std::endl;

      this->Reset();

      this->SetupCamera(
        glm::vec3(0.0f, 0.0f, 10.f),
        glm::vec3(0.0f, 0.0f, -1.0f)
      );

      // setup sceneData sharedpointer

      //this->AddObject<SimplePlane>(
        //myShaders::basicVertex, 
        //myShaders::basicFragment, 
        //glm::scale(glm::identity<glm::mat4>(), glm::vec3(5, 5, 0)), 
        //glm::vec4(1.0, 1.0, 1.0, 1.0)
      //);

      // no 
      this->AddObject<SimpleButton>(
        &this->renderer,
        glm::rotate(glm::scale(glm::identity<glm::mat4>(), glm::vec3(5, 5, 0)), 0.1f, glm::vec3(0, 0, 1.0f)), 
        glm::vec4(1.0, 1.0, 1.0, 1.0)
      );

      // make button, sending them sceneData if needed
      break;
    }
    default: {
      break;
    }
  };

  this->currentScene = sceneIndex;
}
