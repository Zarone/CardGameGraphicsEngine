#include <iostream>
#include "./TestScene.h"
#include "./TestGameState.h"

TestScene::TestScene(WindowManager* windowManager):
  Scene(windowManager)
{
}

void TestScene::Swap(unsigned int sceneIndex) {
  std::cout << "swap request to scene index " << sceneIndex << std::endl;

  switch ((TestSceneID)sceneIndex) {
    case TestSceneID::GAME_SCREEN: {
      std::cout << "swap to game screen" << std::endl;
      break;
    }
    case TestSceneID::GAME_SCREEN_2_TEST: {
      std::cout << "swap to game screen 2" << std::endl;
      this->Reset();
      this->AddObject<TestGameState>();
      break;
    }
    case TestSceneID::MAIN_MENU: {
      std::cout << "swap to main menu" << std::endl;
      break;
    }
    default: {
      break;
    }
  };
}
