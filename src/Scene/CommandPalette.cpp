#include "CommandPalette.h"
#include <iostream>

CommandPalette::CommandPalette(Renderer* renderer): renderer(renderer) {
    try {
        // Initialize text renderer with a default font
        textRenderer = new TextRenderer(renderer, "../assets/fonts/Roboto-Regular.ttf", 24);
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize text renderer: " << e.what() << std::endl;
        textRenderer = nullptr;
    }
}

CommandPalette::~CommandPalette() {
    if (textRenderer) {
        delete textRenderer;
    }
}

float scaleMultiple(int i, int max){
  float inner = ((float)(i))/(max+1);
  return 1-inner*inner;
}

void CommandPalette::SetButtons(std::vector<ButtonArgs> buttons) {
  this->buttons = std::vector<std::pair<const std::string, BoundButton>>();
  int size = buttons.size();
  int renderedSize = fmin(size, maxSize);

  float radius = 0.4f; // Radius of the semicircle
  float startAngle = 0.0f; // Start right of semicircle

  float angleStep = -M_PI / 2 / maxSize; // Evenly space buttons along semicircle
  
  double xScale = 4.0f;
  for (int i = 0; i < renderedSize; i++) {
    float angle = startAngle + (angleStep * i);
    float x = -1.0f + (radius * cos(angle))/this->renderer->GetAspectRatio();
    float y = 0.5 + radius * sin(angle);
    
    float scale = scaleMultiple(i,maxSize)*buttonScale;
    
    glm::mat4 transform = glm::translate(glm::identity<glm::mat4>(), glm::vec3(x, y, i*0.1f+0.00001f));
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
    this->buttons.push_back(
      std::pair<const std::string&, BoundButton>(
        buttons[i].text, 
        std::move(newButton)
      )
    );
  }
}

void CommandPalette::Render(Renderer* renderer) {
  // Render buttons
  for (std::pair<const std::string, BoundButton>& buttonPair : this->buttons) {
    buttonPair.second.Render(renderer);
  }

  // Render text on top of buttons if text renderer is available
  if (textRenderer) {
    for (size_t i = 0; i < this->buttons.size(); i++) {
      const BoundButton& button = this->buttons[i].second;
      const std::string& text = this->buttons[i].first;
      glm::vec4 screenPos = button.GetTransform() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

      // Debug output
      CursorData data;
      renderer->GetCursorPosition(&data);
      
      float scale = scaleMultiple(i,maxSize)*buttonScale*0.02f;
      
      // Center text on button
      float textWidth = textRenderer->GetTextWidth(text, scale);
      float textHeight = textRenderer->GetTextHeight(text, scale);
      float x = screenPos.x - textWidth / 2.0f;
      float y = screenPos.y - textHeight / 2.0f;
      
      glm::vec4 textColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
      textRenderer->RenderText(renderer, text, x, y, screenPos.z, scale, textColor);
    }
  }
}

void CommandPalette::UpdateTick(double deltaTime) {
  for (std::pair<const std::string, BoundButton>& buttonPair : this->buttons) {
    buttonPair.second.UpdateTick(deltaTime);
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

  for (int i = 0; i < buttons.size(); ++i) {
    const BoundButton& button = buttons[i].second;
    CollisionInfo localCollisionInfo;
    double localCollisionZ = MAXFLOAT;
    bool objectCollision = button.CheckCollision(renderer, x, y, &localCollisionZ, &localCollisionInfo);

    if (objectCollision && localCollisionZ < bestCollisionZ) {
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
  return info.innerCollision->groupPointer->ProcessClick(std::move(info));
}

ClickEvent CommandPalette::ProcessPreClick(CollisionInfo info) {
  return {};
}

void CommandPalette::ReleaseClick() {
}
