#pragma once

#include "../include/Shader.h"
#include "../include/TextureMap.h"

struct Material
{
  bool hasTexture;
  TextureMap* textureMap;
  Shader* shader;
  glm::vec4 color;
};
