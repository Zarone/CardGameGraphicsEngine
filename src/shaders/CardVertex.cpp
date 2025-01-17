#include "../../include/shaders/allShaders.h"

std::string myShaders::cardVertex = R"(
#version 330 core

layout(location = 0) in vec4 position;
//layout(location = 1) in vec2 textureCoordinates;
//layout(location = 2) in vec4 positionRelativeGroup;
//layout(location = 3) in float rotation;
//layout(location = 4) in int cardTexture;

//out vec2 fragmentTextureCoordinates;
//flat out int f_cardTexture;

uniform mat4 projMatrix;
uniform mat4 cameraMatrix;
uniform mat4 modelMatrix;

void main()
{
  gl_Position = position;
  //gl_Position = projMatrix * cameraMatrix * modelMatrix * (position+positionRelativeGroup);

  //fragmentTextureCoordinates = textureCoordinates;

  //f_cardTexture = cardTexture;
}
)";

