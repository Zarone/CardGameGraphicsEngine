#include "../include/ErrorHandling.h"
#include "./TestGameState.h"
#include "./TestSceneData.h"

TestGameState::TestGameState():
  hand(
    glm::vec3(0.0f, -2.0f, 4.0f),
    //glm::vec3(0.0f, -0.5f, 6.0f),
    -20.0f, // rotateX
    4.0f,   // width
    false,  // z flipped
    true
  ),
  oppHand(
    glm::vec3(0.0f, 2.0f, 1.0f),
    40.0f, // rotateX
    4.0f,  // width
    true, // z flipped
    true
  )
{
  oppHand.AddCard(0);
  //oppHand.AddCard(2);
  //oppHand.AddCard(3);

  hand.AddCard(0);
  hand.AddCard(1);
  hand.AddCard(2);
  hand.AddCard(3);
  hand.AddCard(0);
  hand.AddCard(1);
  hand.AddCard(2);

  AddObject(&hand);
  AddObject(&oppHand);
}

ClickEvent TestGameState::ProcessClick(CollisionInfo info) {
  CardGroup* src = (CardGroup*) info.groupPointer;
  if (src == &this->hand) {
    std::cout << "hand collision recognized" << std::endl;
    this->hand.MoveToGroup(info.collisionIndex, &this->oppHand);
  } else if (src == &this->oppHand) {
    std::cout << "opp hand collision recognized" << std::endl;
  } else {
    std::cout << "couldn't recognize group pointer" << std::endl;
    ASSERT(false);
  }
  return {
    .sceneSwap = true,
    .sceneIndex = TestSceneID::GAME_SCREEN_2_TEST
  };
}

ClickEvent TestGameState::ProcessPreClick(CollisionInfo info) {
  return {};
}

void TestGameState::ReleaseClick() {
}
