#include "../include/GameState.h"
#include "../include/ErrorHandling.h"

GameState::GameState() :
  hand(
    glm::vec3(0.0f, -2.0f, 4.0f),
    //glm::vec3(0.0f, -0.5f, 6.0f),
    -20.0f, // rotateX
    4.0f, // width
    false,// z flipped
    true
  ),
  oppHand(
    glm::vec3(0.0f, 2.0f, 1.0f),
    40.0f, // rotateX
    4.0f, // width
    true,// z flipped
    true
  )
{
  //oppHand.AddCard(0);
  //oppHand.AddCard(2);
  //oppHand.AddCard(3);

  hand.AddCard(0);
  //hand.AddCard(1);
  //hand.AddCard(2);
  //hand.AddCard(3);
  //hand.AddCard(0);
  //hand.AddCard(1);
  //hand.AddCard(2);
}

void GameState::Render(Renderer* renderer, const RenderData& renderData) {
  this->hand.Render(renderer, renderData);
  this->oppHand.Render(renderer, renderData);
}

void GameState::UpdateTick(double deltaTime) {
  this->hand.UpdateTick(deltaTime);
  this->oppHand.UpdateTick(deltaTime);
}

bool GameState::CheckCollision(Renderer* renderer, double x, double y, double* collisionZ, CollisionInfo* info) const {
  double minZ = MAXFLOAT;
  CollisionInfo collisionInfo = {
    .groupPointer = nullptr
  };

  double tempZ;
  CollisionInfo tempInfo;
  bool objectCollission = this->hand.CheckCollision(renderer, x, y, &tempZ, &tempInfo);

  if (objectCollission && tempZ < minZ) {
    minZ = tempZ;
    collisionInfo = tempInfo;
    collisionInfo.groupPointer = (void*)&hand;
  }

  objectCollission = this->oppHand.CheckCollision(renderer, x, y, &tempZ, &tempInfo);
  if (objectCollission && tempZ < minZ) {
    minZ = tempZ;
    collisionInfo = tempInfo;
    collisionInfo.groupPointer = (void*)&oppHand;
  }

  if (collisionInfo.groupPointer != nullptr) {
    *collisionZ = minZ;
    *info = collisionInfo;
    return true;
  } else {
    return false;
  }

}

void GameState::ProcessClick(CollisionInfo info) {
  CardGroup* src = (CardGroup*) info.groupPointer;
  if (src == &this->hand) {
    //std::cout << "hand collision recognized" << std::endl;
    this->hand.MoveToGroup(info.collisionIndex, &this->oppHand);
  } else if (src == &this->oppHand) {
    //std::cout << "opp hand collision recognized" << std::endl;
  } else {
    std::cout << "couldn't recognize group pointer" << std::endl;
    ASSERT(false);
  }
}
