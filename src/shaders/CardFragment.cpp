#include "../../include/shaders/allShaders.h"

std::string myShaders::cardFragment = R"(
#version 330 core

layout(location = 0) out vec4 color;

in vec2 fragmentTextureCoordinates;

//uniform sampler2D backCardTexture;
//uniform sampler2D frontCardTexture;
uniform sampler2D cardTexture;

void main()
{
  if (gl_FrontFacing) {
    color = texture(cardTexture, fragmentTextureCoordinates);
  } else {
    color = texture(
      cardTexture, vec2(
        1-fragmentTextureCoordinates.x, 
        fragmentTextureCoordinates.y
      )
    );
  }
  
  //if (gl_FrontFacing) {
    //color = texture(frontCardTexture, fragmentTextureCoordinates);
  //} else {
    //color = texture(
      //backCardTexture, vec2(
        //1-fragmentTextureCoordinates.x, 
        //fragmentTextureCoordinates.y
      //)
    //);
  //}
}
)";
