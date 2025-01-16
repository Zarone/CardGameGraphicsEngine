#include "../../include/shaders/allShaders.h"

std::string myShaders::cardVertex = R"(
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 textureCoordinates;

out vec4 vertColor;
out vec2 fragmentTextureCoordinates;

uniform mat4 projMatrix;
uniform mat4 cameraMatrix;
uniform mat4 rotMatrix;

void main()
{
  gl_Position = projMatrix * cameraMatrix * rotMatrix * position;
  vertColor = vec4(position.x+0.5, position.y+0.5, 0, 1);

  fragmentTextureCoordinates = textureCoordinates;
}
)";

