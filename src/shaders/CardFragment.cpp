#include "../../include/shaders/allShaders.h"

std::string myShaders::cardFragment = R"(
#version 330 core

layout(location = 0) out vec4 color;

//in vec2 fragmentTextureCoordinates;
//flat in int f_cardTexture;

//uniform sampler2D textures[16];

void main()
{
  color = vec4(0.0, 1.0, 0.0, 1.0);
  //if (gl_FrontFacing) {
    //color = texture(textures[f_cardTexture], fragmentTextureCoordinates);
  //} else {
    //color = texture(
      //textures[f_cardTexture], vec2(
        //1-fragmentTextureCoordinates.x, 
        //fragmentTextureCoordinates.y
      //)
    //);
  //}
}
)";
