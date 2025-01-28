#include "../../include/shaders/allShaders.h"

std::string myShaders::highlightedCardVertex = R"(
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 textureCoordinates;
layout(location = 2) in vec3 positionRelativeGroup;
layout(location = 3) in float rotation;
layout(location = 4) in float scale;
layout(location = 5) in int cardTexture;

out vec2 v_texture;
out vec3 v_normal;
out vec3 v_cameraPosition;
flat out int f_cardTexture;

uniform mat4 u_projMatrix;
uniform mat4 u_cameraMatrix;
uniform mat4 u_modelMatrix;

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

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
  vec3 vertexPosition = (dynamicRotationMatrix * position).xyz + positionRelativeGroup;

  mat4 cameraModelMatrix = u_cameraMatrix * u_modelMatrix;
  vec4 cameraPosition4 = (cameraModelMatrix * vec4(vertexPosition, 1));
  gl_Position = u_projMatrix * cameraPosition4;
  v_cameraPosition = cameraPosition4.xyz;

  v_normal = normalize(mat3(cameraModelMatrix)*vec3(0,0,1));

  v_texture = textureCoordinates;

  f_cardTexture = cardTexture;
}
)";

