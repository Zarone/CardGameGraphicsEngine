#include "./TestScene.h"
#include "./TestGameState.h"
#include "./TestSceneData.h"
#include "../include/BoundButton.h"

TestScene::TestScene(WindowManager* windowManager, TestCardDatabaseSingleton* database):
  buttonShader(myShaders::basicVertex, myShaders::basicFragment),
  Scene(windowManager, database), database(database)
{
}

void TestScene::Swap(unsigned int sceneIndex) {
  Material buttonMaterial = {
    .hasTexture = false,
    .textureMap = &this->renderer.textureMap,
    .shader = &this->buttonShader,
    .color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
  };
  
  switch ((TestSceneID)sceneIndex) {
    case TestSceneID::GAME_SCREEN: {
      this->Reset();

      this->SetupCamera(
        glm::vec3(0.0f, 4.5f, 9.8f),
        glm::vec3(0.0f, -0.9f, -1.0f)
      );
      this->AddObject<TestGameState>(&this->renderer, this->database);

      break;
    }
    case TestSceneID::MAIN_MENU: {
      this->Reset();

      this->SetupCamera(
        glm::vec3(0.0f, 0.0f, 10.f),
        glm::vec3(0.0f, 0.0f, -1.0f)
      );

      this->AddObject<BoundButton>(
        &this->renderer,
        glm::rotate(
          glm::scale(
            glm::identity<glm::mat4>(), 
            glm::vec3(5, 5, 0)
          ), 0.1f, 
          glm::vec3(0, 0, 1.0f)
        ), 
        buttonMaterial,
        std::bind(&TestScene::Swap, this, TestSceneID::GAME_SCREEN)
      );

      break;
    }
    default: {
      break;
    }
  };

  this->currentScene = sceneIndex;
}
