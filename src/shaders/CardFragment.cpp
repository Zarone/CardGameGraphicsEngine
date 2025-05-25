#include "allShaders.h"

std::string myShaders::cardFragment = R"(
#version 330 core

layout(location = 0) out vec4 color;

in vec2 fragmentTextureCoordinates;
flat in int f_cardTexture;

uniform sampler2D textures[16];

float scale = 0.97;
float superEllipse(vec2 uv)
{
    vec2 p = abs(uv - 0.5) * 2.0;

    for (int i = 0; i < 4; i++) {
      p.x = p.x*p.x;
    }
    for (int i = 0; i < 5; i++) {
      p.y = p.y*p.y;
    }
    
    return p.x + p.y;
}

void main()
{
  vec2 uv = fragmentTextureCoordinates;

  if (superEllipse(uv) > 1.0) {
    discard;
  }

  uv = uv / scale - (1-scale)*0.5;

  if (superEllipse(uv) > 1.0) {
    discard;
  }

  vec4 texColor = (gl_FrontFacing)
      ? texture(textures[f_cardTexture], uv)
      : texture(textures[f_cardTexture], vec2(1.0 - uv.x, uv.y));

  color = texColor;
}
)";
