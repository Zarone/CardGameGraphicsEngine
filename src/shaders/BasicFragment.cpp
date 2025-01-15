#include "../../include/shaders/allShaders.h"

std::string myShaders::basicFragment = R"(
#version 330 core

layout(location = 0) out vec4 color;
in vec4 vertColor;

void main()
{
  //color = vec4(1.0, 0.0, 0.0, 1.0);
  color = vertColor;
}
)";
