#include "../../include/shaders/allShaders.h"

std::string myShaders::textureVertex = R"(
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texturePosition;
//out vec4 vertColor;
out vec2 fragmentTextureCoordinates;

uniform mat4 projMatrix;
uniform mat4 cameraMatrix;
uniform mat4 modelMatrix;
uniform vec4 color;

void main()
{
  gl_Position = projMatrix * cameraMatrix * modelMatrix * position;
  //vertColor = color;

  fragmentTextureCoordinates = texturePosition;
}
)";
