#include "CommandPalette.h"

CommandPalette::CommandPalette(Renderer* renderer): renderer(renderer){};

void CommandPalette::SetButtons(std::vector<ButtonArgs> buttons) {
  this->buttons = std::vector<BoundButton>();
  int size = buttons.size();
  int renderedSize = fmin(size, 3);

  float radius = 0.4f; // Radius of the semicircle
  float startAngle = 0.0f; // Start right of semicircle

  float angleStep = -M_PI / 2 / renderedSize; // Evenly space buttons along semicircle
  
  double xScale = 2.0f;
  for (int i = 0; i < renderedSize; i++) {
    float angle = startAngle + (angleStep * i);
    float x = -1.0f + (radius * cos(angle))/this->renderer->GetAspectRatio();
    float y = 0.5 + radius * sin(angle);
    
    glm::mat4 transform = glm::translate(glm::identity<glm::mat4>(), glm::vec3(x, y, 0.0f));
    double scale = 0.1f;
    transform = glm::scale(transform, glm::vec3(xScale*scale/this->renderer->GetAspectRatio(), scale, 1.0f));
    BoundButton newButton = BoundButton(
      this->renderer,
      transform,
      Material({
        .hasTexture=false,
        .shader=renderer->GetShader(BasicShader),
        .color=glm::vec4(0.7f, 0.7f, 0.75f, 1.0f),
      }),
      buttons[i].func
    );
    newButton.TogglePerspective(false);
    this->buttons.push_back(std::move(newButton));
  }
}

void CommandPalette::Render(Renderer* renderer) {
  for (BoundButton& button : this->buttons) {
    button.Render(renderer);
  }
}

void CommandPalette::UpdateTick(double deltaTime) {
  for (BoundButton& button : this->buttons) {
    button.UpdateTick(deltaTime);
  }
}

bool CommandPalette::CheckCollision(
  Renderer* renderer, 
  double x, 
  double y, 
  double* collisionZ, 
  CollisionInfo* collisionInfo 
) const {
  CollisionInfo bestCollisionInfo;
  double bestCollisionZ = MAXFLOAT;

  const SceneObject* selectedObject = nullptr;

  for (const BoundButton& button : this->buttons) {
    CollisionInfo localCollisionInfo;
    double localCollisionZ = MAXFLOAT;
    bool objectCollision = button.CheckCollision(renderer, x, y, &localCollisionZ, &localCollisionInfo);

    if (objectCollision && localCollisionZ < bestCollisionZ) {
      std::cout << "objectCollision" << std::endl;
      bestCollisionZ = localCollisionZ;
      selectedObject = &button;
      bestCollisionInfo = std::move(localCollisionInfo);
    }
  }

  if (selectedObject == nullptr) return false;

  collisionInfo->innerCollision = std::make_unique<CollisionInfo>();
  collisionInfo->innerCollision->groupPointer = bestCollisionInfo.groupPointer;
  collisionInfo->innerCollision->cardIndex = bestCollisionInfo.cardIndex;
  collisionInfo->innerCollision->isCard = bestCollisionInfo.isCard;
  collisionInfo->innerCollision->innerCollision = std::move(bestCollisionInfo.innerCollision);

  collisionInfo->isCard = false;
  collisionInfo->groupPointer = (SceneObject*) this;

  *collisionZ = bestCollisionZ;

  return true;
}

ClickEvent CommandPalette::ProcessClick(CollisionInfo info) {
  std::cout << "Unimplemented ProcessClick" << std::endl;
  return info.innerCollision->groupPointer->ProcessClick(std::move(info));
};
ClickEvent CommandPalette::ProcessPreClick(CollisionInfo info) {
  std::cout << "Unimplemented ProcessPreClick" << std::endl;
  return {};
};
void CommandPalette::ReleaseClick() {
  std::cout << "Unimplemented ReleaseClick" << std::endl;
}
