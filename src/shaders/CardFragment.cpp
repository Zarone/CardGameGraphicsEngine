#include "../../include/shaders/allShaders.h"

std::string myShaders::cardFragment = R"(
#version 330 core

layout(location = 0) out vec4 color;
in vec4 vertColor;
in vec2 fragmentTextureCoordinates;

uniform sampler2D backCardTexture;
uniform sampler2D frontCardTexture;

void main()
{
  if (gl_FrontFacing) {
    color = texture(frontCardTexture, fragmentTextureCoordinates);
  } else {
    color = texture(
      backCardTexture, vec2(
        1-fragmentTextureCoordinates.x, 
        fragmentTextureCoordinates.y
      )
    );
  }
}
)";
