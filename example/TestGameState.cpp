#include "../include/ErrorHandling.h"
#include "./TestGameState.h"
#include "./TestSceneData.h"
#include "./TestCardInfo.h"

TestGameState::TestGameState(TestCardDatabaseSingleton* database):
  hand(
    glm::vec3(0.0f, -1.5f, 6.25f),
    //glm::vec3(0.0f, -0.5f, 6.0f),
    -30.0f, // rotateX
    4.0f,   // width
    false,  // z flipped
    true
  ),
  oppHand(
    glm::vec3(0.0f, -1.5f, -6.25f),
    10.0f, // rotateX
    4.0f,  // width
    true, // z flipped
    true
  ),
  specials(
    glm::vec3(0.0f, -2.0f, 4.5f),
    //glm::vec3(0.0f, -0.5f, 6.0f),
    -90.0f, // rotateX
    8.0f,   // width
    false,  // z flipped
    false
  ),
  oppSpecials(
    glm::vec3(0.0f, -2.0f, -4.5f),
    //glm::vec3(0.0f, -0.5f, 6.0f),
    -90.0f, // rotateX
    8.0f,   // width
    false,  // z flipped
    false
  ),
  reserve(
    glm::vec3(0.0f, -2.0f, 2.75f),
    //glm::vec3(0.0f, -0.5f, 6.0f),
    -90.0f, // rotateX
    8.0f,   // width
    false,  // z flipped
    false
  ),
  oppReserve(
    glm::vec3(0.0f, -2.0f, -2.75f),
    //glm::vec3(0.0f, -0.5f, 6.0f),
    -90.0f, // rotateX
    8.0f,   // width
    false,  // z flipped
    false
  ),
  battlefield(
    glm::vec3(0.0f, -2.0f, 1.f),
    //glm::vec3(0.0f, -0.5f, 6.0f),
    -90.0f, // rotateX
    8.0f,   // width
    false,  // z flipped
    false
  ),
  oppBatlefield(
    glm::vec3(0.0f, -2.0f, -1.f),
    //glm::vec3(0.0f, -0.5f, 6.0f),
    -90.0f, // rotateX
    8.0f,   // width
    false,  // z flipped
    false
  ),
  database(database)
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
  AddObject(&reserve);
  AddObject(&oppReserve);
  AddObject(&battlefield);
  AddObject(&oppBatlefield);
  AddObject(&specials);
  AddObject(&oppSpecials);
}

ClickEvent TestGameState::ProcessClick(CollisionInfo info) {
  CardGroup* src = (CardGroup*) info.groupPointer;

  if (src == &this->hand) {
    std::cout << "hand collision recognized" << std::endl;

    unsigned int cardIndex = info.collisionIndex;
    Card card = this->hand.GetCard(cardIndex);
    TestCardInfo* cardInfo = this->database->GetInfo(card.GetID());
    if (cardInfo->type == TestCardInfo::CardType::BASIC_CHARACTER_CARD) {
      std::cout << "playing basic character" << std::endl;
       this->hand.MoveToGroup(info.collisionIndex, &this->reserve);
    } else if (cardInfo->type == TestCardInfo::CardType::SPECIAL_CHARACTER_CARD) {
      std::cout << "how do you have that in your hand... :(" << std::endl;
    }

  } else if (src == &this->oppHand) {
    std::cout << "opp hand collision recognized" << std::endl;
  } else if (src == &this->reserve) {
    std::cout << "reserve click recognized" << std::endl;
  } else {
    std::cout << "couldn't recognize group pointer" << std::endl;
    ASSERT(false);
  }

  return {
    .sceneSwap = false,
  };
}

ClickEvent TestGameState::ProcessPreClick(CollisionInfo info) {
  return {};
}

void TestGameState::ReleaseClick() {
}
