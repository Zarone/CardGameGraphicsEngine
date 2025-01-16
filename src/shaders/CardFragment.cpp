#include "../../include/shaders/allShaders.h"

std::string myShaders::cardFragment = R"(
#version 330 core

layout(location = 0) out vec4 color;
in vec4 vertColor;
in vec2 fragmentTextureCoordinates;

uniform sampler2D cardTexture;

void main()
{
  color = texture(cardTexture, fragmentTextureCoordinates);
  //color = vec4(1.0, 1.0, 1.0, 1.0);
}
)";
