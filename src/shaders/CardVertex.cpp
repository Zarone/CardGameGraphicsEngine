#include "../../include/shaders/allShaders.h"

std::string myShaders::cardVertex = R"(
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 textureCoordinates;

out vec2 fragmentTextureCoordinates;

uniform mat4 projMatrix;
uniform mat4 cameraMatrix;
uniform mat4 modelMatrix;

void main()
{
  gl_Position = projMatrix * cameraMatrix * modelMatrix * position;

  fragmentTextureCoordinates = textureCoordinates;
}
)";

