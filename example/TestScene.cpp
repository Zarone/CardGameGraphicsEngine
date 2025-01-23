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
  Shader buttonShader(myShaders::basicVertex, myShaders::basicFragment);
  Material buttonMaterial = {
    .textureMap = &this->renderer.textureMap,
    .hasTexture = false,
    .color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
    .shader = &this->buttonShader
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

      // setup sceneData sharedpointer

      //this->AddObject<SceneSwapButton>(
        //&this->renderer,
        //glm::rotate(
          //glm::scale(
            //glm::identity<glm::mat4>(), 
            //glm::vec3(5, 5, 0)
          //), 0.1f, 
          //glm::vec3(0, 0, 1.0f)
        //), 
        //glm::vec4(1.0, 1.0, 1.0, 1.0),
        //TestSceneID::GAME_SCREEN
      //);
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

      // make button, sending them sceneData if needed
      break;
    }
    default: {
      break;
    }
  };

  this->currentScene = sceneIndex;
}
