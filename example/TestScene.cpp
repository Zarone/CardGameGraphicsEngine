#include "./TestScene.h"
#include "./TestGameState.h"
#include "./TestSceneData.h"
#include "../include/SceneSwapButton.h"

TestScene::TestScene(WindowManager* windowManager, TestCardDatabaseSingleton* database):
  Scene(windowManager, database), database(database)
{
}

void TestScene::Swap(unsigned int sceneIndex) {
  switch ((TestSceneID)sceneIndex) {
    case TestSceneID::GAME_SCREEN: {
      this->Reset();

      this->SetupCamera(
        glm::vec3(0.0f, 7.0f, 11.f),
        glm::vec3(0.0f, -0.9f, -1.0f)
      );
      this->AddObject<TestGameState>(this->database);

      break;
    }
    case TestSceneID::MAIN_MENU: {
      this->Reset();

      this->SetupCamera(
        glm::vec3(0.0f, 0.0f, 10.f),
        glm::vec3(0.0f, 0.0f, -1.0f)
      );

      // setup sceneData sharedpointer

      this->AddObject<SceneSwapButton>(
        &this->renderer,
        glm::rotate(
          glm::scale(
            glm::identity<glm::mat4>(), 
            glm::vec3(5, 5, 0)
          ), 0.1f, 
          glm::vec3(0, 0, 1.0f)
        ), 
        glm::vec4(1.0, 1.0, 1.0, 1.0),
        TestSceneID::GAME_SCREEN
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
