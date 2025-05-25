#include "./TestScene.h"
#include "./TestGameState.h"
#include "./TestSceneData.h"
#include "../src/SimpleRenderObjects/BoundButton.h"

TestScene::TestScene(WindowManager* windowManager, TestCardDatabaseSingleton* database):
  Scene(windowManager, database), database(database)
{
}

void TestScene::Swap(unsigned int sceneIndex) {
  
  this->Reset();
  this->renderer.ResetShaders();
  switch ((TestSceneID)sceneIndex) {
    case TestSceneID::GAME_SCREEN: {
      // Side View Camera
      this->SetupCamera(
        glm::vec3(0.0f, 4.5f, 9.8f),
        glm::vec3(0.0f, -0.9f, -1.0f)
      );
      
      // Top Down Camera
      //this->SetupCamera(
        //glm::vec3(0.0f, 15.0f, 0.0f),
        //glm::vec3(0.0f, -1.0f, -0.01f)
      //);
      
      // setup shader for card and button
      this->renderer.SetupShader("buttonShader", myShaders::textureVertex, myShaders::textureFragment);
      this->renderer.SetupShader("cardShader", myShaders::cardVertex, myShaders::cardFragment);
      this->renderer.SetupShader("highlightCardShader", myShaders::highlightedCardVertex, myShaders::highlightedCardFragment);
      this->renderer.SetupShader("basicShader", myShaders::basicVertex, myShaders::basicFragment);
      
      this->AddObject<TestGameState>(&this->renderer, this->database);

      break;
    }
    case TestSceneID::MAIN_MENU: {
      this->SetupCamera(
        glm::vec3(0.0f, 0.0f, 10.f),
        glm::vec3(0.0f, 0.0f, -1.0f)
      );

      this->renderer.SetupShader("buttonShader", myShaders::textureVertex, myShaders::textureFragment);
      this->renderer.textureMap.SetupTexturePath("startgame");
      this->AddObject<BoundButton>(
        &this->renderer,
        glm::rotate(
          glm::scale(
            glm::identity<glm::mat4>(), 
            glm::vec3(5, 5, 0)
          ), 0.1f, 
          glm::vec3(0, 0, 1.0f)
        ), 
        Material {
          .hasTexture=true,
          .textureMap=&renderer.textureMap,
          .textureAddr="startgame",
          .shader=renderer.GetShader("buttonShader"),
          .color=glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        },
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
