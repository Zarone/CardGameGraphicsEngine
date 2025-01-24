#include "../../include/shaders/allShaders.h"

std::string myShaders::textureFragment = R"(
#version 330 core

layout(location = 0) out vec4 color;
in vec2 fragmentTextureCoordinates;

uniform sampler2D textureSampler;

void main()
{
  color = vec4(texture(textureSampler, vec2(
    fragmentTextureCoordinates.x,
    fragmentTextureCoordinates.y
  )).xyz, color.w);
  //color = texture(textureSampler, vec2(
    //fragmentTextureCoordinates.x,
    //fragmentTextureCoordinates.y
  //));
}
)";
