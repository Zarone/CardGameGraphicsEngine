#include "TemporaryCardGroup.h"
#include <algorithm>

TemporaryCardGroup::TemporaryCardGroup(
    Renderer* renderer,
    glm::vec3 position
):
  showButton(
    renderer,
    glm::identity<glm::mat4>(),
    Material {
      .hasTexture = false,
      .textureMap = &renderer->textureMap,
      .shader = renderer->GetShader(BasicShader),
      .color = glm::vec4(0.9f, 0.0f, 0.0f, 1.0f),
    },
    [this](){this->isExpanded = true;} 
  ),
  expandedRenderer(
    renderer,
    std::bind(&TemporaryCardGroup::OnClose, this)
  ),
  CardGroup(renderer)
{}

void TemporaryCardGroup::OnClose() {
  this->isExpanded = false;
  this->expandedRenderer.SetDirtyPosition(true);
}

void TemporaryCardGroup::Render(Renderer* renderer) {
  if (!this->isEnabled) return;

  if (this->isExpanded) {
    this->expandedRenderer.SetCardsPointer(&this->cards);
    this->expandedRenderer.Render(renderer);
  } else {
    this->showButton.Render(renderer);
  }
}

void TemporaryCardGroup::UpdateTick(double deltaTime) {
  if (!this->isEnabled) return;

  if (isExpanded) {
    this->expandedRenderer.SetCardsPointer(&this->cards);
    this->expandedRenderer.UpdateTick(deltaTime);
  } else {
    this->showButton.UpdateTick(deltaTime);
  }
}

ClickEvent TemporaryCardGroup::ProcessClick(CollisionInfo info) {
  if (!this->isEnabled) {
    throw std::runtime_error("Process Click on disabled Card Group");
  }

  if (isExpanded) {
    return this->expandedRenderer.ProcessClick(std::move(info));
  } else {
    this->isExpanded = true;
    this->expandedRenderer.SetDirtyPosition(true);
    return this->showButton.ProcessClick(std::move(info));
  }
}

void TemporaryCardGroup::ProcessScroll(CollisionInfo info, double yOffset) {
  if (isExpanded) {
    return this->expandedRenderer.ProcessScroll(std::move(info), yOffset);
  }
}

ClickEvent TemporaryCardGroup::ProcessPreClick(CollisionInfo info) {
  if (isExpanded) {
    this->expandedRenderer.ProcessPreClick(std::move(info));
    return {};
  } else {
    return this->showButton.ProcessPreClick(std::move(info));
  }
}

void TemporaryCardGroup::ReleaseClick(){
  if (isExpanded) {
    this->expandedRenderer.ReleaseClick();
  } else {
    this->showButton.ReleaseClick();
  }
}

bool TemporaryCardGroup::CheckCollision(
  Renderer* renderer, 
  double x, 
  double y, 
  double* collisionZ, 
  CollisionInfo* info
) const {
  if (!this->isEnabled) return false;

  info->groupPointer = (TemporaryCardGroup*)this;
  if (isExpanded) {
    bool collision = this->expandedRenderer.CheckCollision(renderer, x, y, collisionZ, info);
    info->groupPointer = (CardGroup*) this;
    return collision;
  } else {
    return this->showButton.CheckCollision(renderer, x, y, collisionZ, info);
  }
}

void TemporaryCardGroup::SetDirtyPosition(bool dirty) {
  if (isExpanded) {
    this->expandedRenderer.SetDirtyPosition(dirty);
    return;
  }
}

const glm::mat4 TemporaryCardGroup::WorldSpaceToThisSpace() {
  if (isExpanded) {
    return this->expandedRenderer.WorldSpaceToThisSpace();
  } else {
    throw std::runtime_error("Running WorldSpaceToThisSpace in unexpanded TemporaryCardGroup");
    return glm::identity<glm::mat4>();
  }
}

void TemporaryCardGroup::MoveToGroup(int index, CardGroup* to) {
  throw std::runtime_error("Unimplemented MoveToGroup for TemporaryCardGroup");
}

void TemporaryCardGroup::EnableWithCards(const std::vector<unsigned int> cards) {
  this->cards = {};
  for (unsigned int card : cards) {
    this->cards.push_back({
      .card = Card(card, -1),
      .renderData = CardRenderingData()
    });
  }
  //std::transform(std::begin(cards), std::end(cards), std::begin(this->cards), 
    //[](unsigned int cardID) -> CardItem { 
      //return {
        //.card = cardID,
        //.renderData = CardRenderingData()
      //};
    //}
  //);
  PrintVector(std::cout, cards);
  PrintVector(std::cout, this->cards);
  std::cout << "Here a ds d" << std::endl;
  this->expandedRenderer.SetCardsPointer(&this->cards);
  this->expandedRenderer.SetDirtyPosition(true);
  this->isEnabled = true;
  this->isExpanded = true;
}

void TemporaryCardGroup::Disable() {
  this->isEnabled = false;
}

bool const TemporaryCardGroup::GetIsExpanded() {
  return this->isExpanded;
}


