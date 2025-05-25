#include "allShaders.h"

std::string myShaders::cardVertex = R"(
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 textureCoordinates;
layout(location = 2) in vec3 positionRelativeGroup;
layout(location = 3) in float rotation;
layout(location = 4) in float scale;
layout(location = 5) in int cardTexture;

out vec2 fragmentTextureCoordinates;
flat out int f_cardTexture;

uniform mat4 u_projMatrix;
uniform mat4 u_cameraMatrix;
uniform mat4 u_modelMatrix;

mat4 getZRotation(float rotation) 
{
  float sinR = sin(rotation);
  float cosR = cos(rotation);
  return mat4(
    cosR, -sinR, 0, 0,
    sinR, cosR, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
  );
}

void main()
{
  mat4 dynamicRotationMatrix = getZRotation(rotation);
  vec3 vertexPosition = scale*((dynamicRotationMatrix * position).xyz) + positionRelativeGroup;

  gl_Position = 
    u_projMatrix * u_cameraMatrix * u_modelMatrix 
    * vec4(vertexPosition, 1);

  fragmentTextureCoordinates = textureCoordinates;

  f_cardTexture = cardTexture;
}
)";

