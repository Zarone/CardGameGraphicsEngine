#include "FannedCardGroup.h"
#include <memory>

FannedCardGroup::FannedCardGroup(
    Renderer* renderer,
    glm::vec3 position, 
    float rotationX, 
    float width, 
    bool zFlipped,
    bool isHand
):
  thisRenderer(
    renderer,
    position,
    rotationX,
    width,
    zFlipped,
    isHand
  ),
  CardGroup(renderer)
{}

void FannedCardGroup::Render(Renderer* renderer) {
  this->thisRenderer.SetCardsPointer(&this->cards);
  this->thisRenderer.Render(renderer);
}

void FannedCardGroup::UpdateTick(double deltaTime) {
  this->thisRenderer.UpdateTick(deltaTime);
}

ClickEvent FannedCardGroup::ProcessClick(CollisionInfo info) {
  return this->thisRenderer.ProcessClick(std::move(info));
}

ClickEvent FannedCardGroup::ProcessPreClick(CollisionInfo info) {
  return this->thisRenderer.ProcessClick(std::move(info));
}

void FannedCardGroup::ReleaseClick(){
  this->thisRenderer.ReleaseClick();
}

bool FannedCardGroup::CheckCollision(
  Renderer* renderer, 
  double x, 
  double y, 
  double* collisionZ, 
  CollisionInfo* info
) const {
  if (this->thisRenderer.CheckCollision(renderer, x, y, collisionZ, info)) {
    CollisionInfo newInfo = {
      .isCard = false,
      .groupPointer = (CardGroup*) this,
    };

    newInfo.innerCollision = std::make_unique<CollisionInfo>();
    newInfo.innerCollision->innerCollision = std::move(info->innerCollision);
    newInfo.innerCollision->isCard = info->isCard;
    newInfo.innerCollision->cardIndex = info->cardIndex;
    newInfo.innerCollision->groupPointer = info->groupPointer;

    info->groupPointer = newInfo.groupPointer;
    info->isCard = newInfo.isCard;
    info->innerCollision = std::move(newInfo.innerCollision);
    info->cardIndex = newInfo.cardIndex;

    return true;
  }
  return false;
}

void FannedCardGroup::SetNumHighlightedCards(int num){
  this->thisRenderer.highlightedCards=num;
}

void FannedCardGroup::SetDirtyPosition(bool dirty) {
  this->thisRenderer.SetDirtyPosition(dirty); 
}

const glm::mat4 FannedCardGroup::WorldSpaceToThisSpace() {
  return this->thisRenderer.WorldSpaceToThisSpace();
}

void FannedCardGroup::MoveToGroup(int index, CardGroup* to) {
  this->SetDirtyPosition(true);
  to->SetDirtyPosition(true);

  // this just makes it so that
  // in the next render we reset
  // the selected index
  this->thisRenderer.wasInsideBoundary = false;

  glm::vec4 v = glm::vec4(
    cards[index].renderData.displayedPosition.x,
    cards[index].renderData.displayedPosition.y,
    cards[index].renderData.displayedPosition.z,
    1
  );

  CardItem cardCopy = {
    .card = cards[index].card,
  };
  std::cout << "initial game id: " << cards[index].card.GetGameID() << std::endl;
  std::cout << "new game id: " << cardCopy.card.GetGameID() << std::endl;
  cardCopy.renderData.displayedPosition = (to->WorldSpaceToThisSpace())*this->thisRenderer.transform*v;

  to->AddCard(cardCopy);
  cards.erase(cards.begin() + index);
}

