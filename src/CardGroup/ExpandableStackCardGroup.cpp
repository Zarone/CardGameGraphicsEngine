#include "ExpandableStackCardGroup.h"

ExpandableStackCardGroup::ExpandableStackCardGroup(
    Renderer* renderer,
    glm::vec3 position, 
    float rotationX, 
    bool zFlipped
):
  stackRenderer(
    renderer,
    position,
    rotationX,
    zFlipped
  ),
  expandedRenderer(
    renderer,
    std::bind(&ExpandableStackCardGroup::OnClose, this)
  ),
  CardGroup(renderer)
{}

void ExpandableStackCardGroup::OnClose() {
  this->isExpanded = false;
  this->stackRenderer.SetDirtyPosition(true);
  this->expandedRenderer.SetDirtyPosition(true);

  /* This has the effect of sending the cards from the camera frame
   * into the 3D space but it looks janky so I'm just sending it to 
   * 0,0,0 now
   
  // make each card in the world space reference frame, and then
  // the reference frame of the stack renderer.
  glm::mat4 transformation = this->stackRenderer.WorldSpaceToThisSpace() 
    * glm::inverse(renderer->projMatrix*renderer->cameraMatrix)
    * this->expandedRenderer.transform;
  */

  for (auto& card : this->cards) {
    /* 
     * 
     * This uses the old transformation from camera space
     *

    glm::vec4 v = glm::vec4(
      card.renderData.displayedPosition.x,
      card.renderData.displayedPosition.y,
      card.renderData.displayedPosition.z,
      1
    );

    card.renderData.displayedPosition = transformation * v;
    */
    
    /* This just uses 0,0,0, which looks better anyway */
    card.renderData.displayedScaleXY = 1;
    card.renderData.displayedRotationZ = 0;
    card.renderData.displayedPosition = glm::vec4(0.5,0,1,1);
  }

}

void ExpandableStackCardGroup::Render(Renderer* renderer) {
  if (isExpanded) {
    this->expandedRenderer.SetCardsPointer(&this->cards);
    this->expandedRenderer.Render(renderer);
  } else {
    this->stackRenderer.SetCardsPointer(&this->cards);
    this->stackRenderer.Render(renderer);
  }
}

void ExpandableStackCardGroup::UpdateTick(double deltaTime) {
  if (isExpanded) {
    this->expandedRenderer.SetCardsPointer(&this->cards);
    this->expandedRenderer.UpdateTick(deltaTime);
  } else {
    this->stackRenderer.SetCardsPointer(&this->cards);
    this->stackRenderer.UpdateTick(deltaTime);
  }
}

ClickEvent ExpandableStackCardGroup::ProcessClick(CollisionInfo info) {
  if (isExpanded) {
    return this->expandedRenderer.ProcessClick(std::move(info));
  } else {
    this->isExpanded = true;
    this->stackRenderer.SetDirtyPosition(true);
    this->expandedRenderer.SetDirtyPosition(true);
    return this->stackRenderer.ProcessClick(std::move(info));
  }
}

void ExpandableStackCardGroup::ProcessScroll(CollisionInfo info, double yOffset) {
  if (isExpanded) {
    return this->expandedRenderer.ProcessScroll(std::move(info), yOffset);
  }
}

ClickEvent ExpandableStackCardGroup::ProcessPreClick(CollisionInfo info) {
  if (isExpanded) {
    this->expandedRenderer.ProcessPreClick(std::move(info));
    return {};
  } else {
    return this->stackRenderer.ProcessPreClick(std::move(info));
  }
}

void ExpandableStackCardGroup::ReleaseClick(){
  if (isExpanded) {
    this->expandedRenderer.ReleaseClick();
  } else {
    this->stackRenderer.ReleaseClick();
  }
}

bool ExpandableStackCardGroup::CheckCollision(
  Renderer* renderer, 
  double x, 
  double y, 
  double* collisionZ, 
  CollisionInfo* info
) const {
  info->groupPointer = (ExpandableStackCardGroup*)this;
  if (isExpanded) {

    bool collision = this->expandedRenderer.CheckCollision(renderer, x, y, collisionZ, info);
    info->groupPointer = (CardGroup*) this;
    return collision;
  } else {
    return this->stackRenderer.CheckCollision(renderer, x, y, collisionZ, info);
  }
}

void ExpandableStackCardGroup::SetDirtyPosition(bool dirty) {
  if (isExpanded) {
    this->expandedRenderer.SetDirtyPosition(dirty);
    return;
  } else {
    this->stackRenderer.SetDirtyPosition(dirty); 
  }
}

const glm::mat4 ExpandableStackCardGroup::WorldSpaceToThisSpace() {
  if (isExpanded) {
    return this->expandedRenderer.WorldSpaceToThisSpace();
  } else {
    return this->stackRenderer.WorldSpaceToThisSpace();
  }
}

void ExpandableStackCardGroup::MoveToGroup(int index, CardGroup* to) {
  this->SetDirtyPosition(true);
  to->SetDirtyPosition(true);

  glm::vec4 v = glm::vec4(
    cards[index].renderData.displayedPosition.x,
    cards[index].renderData.displayedPosition.y,
    cards[index].renderData.displayedPosition.z,
    1
  );

  CardItem cardCopy = {
    .card = cards[index].card,
  };
  if (this->isExpanded) {
    std::cout << "unimplemented expansion in movetogroup" << std::endl;
    //ASSERT(false);
  } else {
    cardCopy.renderData.displayedPosition = (to->WorldSpaceToThisSpace())*this->stackRenderer.transform*v;
  }

  to->AddCard(cardCopy);
  cards.erase(cards.begin() + index);
}

bool const ExpandableStackCardGroup::GetIsExpanded() {
  return this->isExpanded;
}
