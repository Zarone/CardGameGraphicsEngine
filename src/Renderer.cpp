#include "../include/Renderer.h"

Renderer::Renderer(WindowManager* window): 
  window(window),
  maxBindableTextures(window->GetMaxBindableTextures()),
  textureMap()
{
}

void Renderer::Setup3DTransforms(const glm::vec3& cameraPosition, const glm::vec3& cameraDirection) {
  this->projMatrix = glm::perspective(
    glm::radians(45.0f), window->AspectRatio(),
    0.1f, 1000.0f
  );
  this->cameraMatrix= glm::lookAt(
    cameraPosition,
    cameraPosition+cameraDirection,
    glm::vec3(0.0f, 1.0f, 0.0f)
  );
}

void Renderer::SetupDatabaseForTexturing(CardDatabaseSingleton* database) {
  this->textureMap.SetupCardDataBase(database);
}

bool Renderer::InsideWindowBounds(double posX, double posY) const {
  return this->window->CursorInBounds(posX, posY);
}

glm::vec2 Renderer::GetScreenPositionFromCamera(glm::vec4 screenCoords) {
  return glm::vec2(
    (screenCoords.x/screenCoords.w+1)/2*window->GetWidth(),
    (-screenCoords.y/screenCoords.w+1)/2*window->GetHeight()
  );
}

glm::vec3 Renderer::Get3DScreenPositionFromCamera(glm::vec4 screenCoords) {
  return glm::vec3(
    (screenCoords.x/screenCoords.w+1)/2*window->GetWidth(),
    (-screenCoords.y/screenCoords.w+1)/2*window->GetHeight(),
    screenCoords.z/screenCoords.w
  );
}

void Renderer::GetCursorPosition(CursorData* cursorData) {
  this->window->GetCursorPosition(&cursorData->cursorX, &cursorData->cursorY);
}

void Renderer::SetupShader(const std::string& shaderName, const std::string& vertex, const std::string& fragment) {
  std::cout << "setup " << shaderName << std::endl;
  this->shaderMap.emplace(
    std::piecewise_construct,
    std::forward_as_tuple(shaderName),
    std::forward_as_tuple(vertex, fragment)
  );
}

void Renderer::ResetShaders() {
  this->shaderMap = std::unordered_map<std::string, Shader>();
}

Shader* Renderer::GetShader(const std::string& name) {
  auto shader = this->shaderMap.find(name);
  if (shader == this->shaderMap.end()) {
    std::cout << "Could not find shader " << name << std::endl;
    return nullptr;
  } else {
    return &(shader->second);
  }
}
