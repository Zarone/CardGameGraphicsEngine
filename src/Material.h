#pragma once

#include "Shader.h"
#include "TextureMap.h"

struct Material
{
  bool hasTexture;
  TextureMap* textureMap;
  std::string textureAddr;
  Shader* shader;
  glm::vec4 color;
};
