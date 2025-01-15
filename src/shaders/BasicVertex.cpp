#include "../../include/shaders/allShaders.h"

std::string myShaders::basicVertex = R"(
#version 330 core

layout(location = 0) in vec4 position;
out vec4 vertColor;

uniform mat4 projMatrix;
uniform mat4 cameraMatrix;
uniform mat4 rotationMatrix;

void main()
{
  gl_Position = projMatrix * cameraMatrix * rotationMatrix * position;
  vertColor = vec4(position.x+0.5, position.y+0.5, 0, 1);
}
)";
