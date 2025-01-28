#include "../../include/CardGroup/ExpandableStackCardGroup.h"

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
    renderer
  )
{}

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
    this->expandedRenderer.UpdateTick(deltaTime);
  } else {
    this->stackRenderer.UpdateTick(deltaTime);
  }
}

ClickEvent ExpandableStackCardGroup::ProcessClick(CollisionInfo info) {
  if (isExpanded) {
    std::cout << "You forgot to implement expanded processClick" << std::endl;
    return {};
  } else {
    this->isExpanded = true;
    return this->stackRenderer.ProcessClick(info);
  }
}

ClickEvent ExpandableStackCardGroup::ProcessPreClick(CollisionInfo info) {
  if (isExpanded) {
    std::cout << "You forgot to implement expanded processpreclick" << std::endl;
    return {};
  } else {
    return this->stackRenderer.ProcessPreClick(info);
  }
}

void ExpandableStackCardGroup::ReleaseClick(){
  if (isExpanded) {
    std::cout << "You forgot to implement expanded releaseClick" << std::endl;
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
  if (isExpanded) {
    std::cout << "You forgot to implement expanded checkCollision" << std::endl;
    return false;
  } else {
    return this->stackRenderer.CheckCollision(renderer, x, y, collisionZ, info);
  }
}

void ExpandableStackCardGroup::SetNumHighlightedCards(int num){
  if (isExpanded) {
    std::cout << "You forgot to implement expanded setnumhighlightedcards" << std::endl;
    return;
  } else {
    this->stackRenderer.highlightedCards=num;
  }
}

void ExpandableStackCardGroup::SetDirtyPosition(bool dirty) {
  if (isExpanded) {
    std::cout << "You forgot to implement expanded setDirtyPosition" << std::endl;
    return;
  } else {
    this->stackRenderer.SetDirtyPosition(dirty); 
  }
}

const glm::mat4 ExpandableStackCardGroup::WorldSpaceToThisSpace() {
  if (isExpanded) {
    std::cout << "You forgot to implement expanded worldSpaceToThisSpace" << std::endl;
    ASSERT(false);
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
    ASSERT(false);
  } else {
    cardCopy.renderData.displayedPosition = (to->WorldSpaceToThisSpace())*this->stackRenderer.transform*v;
  }

  to->AddCard(cardCopy);
  cards.erase(cards.begin() + index);
}

