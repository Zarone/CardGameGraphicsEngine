#include "allShaders.h"

std::string myShaders::basicVertex = R"(
#version 330 core

layout(location = 0) in vec4 position;
out vec4 vertColor;

uniform mat4 projMatrix;
uniform mat4 cameraMatrix;
uniform mat4 modelMatrix;
uniform vec4 color;

float mod(float a, float b)
{
  return a - (b * floor(a/b));
}

void main()
{
  gl_Position = projMatrix * cameraMatrix * modelMatrix * position;
  vertColor = color;
}
)";
