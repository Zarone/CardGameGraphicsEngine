#include "../../include/shaders/allShaders.h"

std::string myShaders::highlightedCardFragment = R"(
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_texture;
in vec3 v_normal;
in vec3 v_cameraPosition;
flat in int f_cardTexture;

uniform sampler2D textures[16];

float shininess = 10;
float glow = 0.8;
vec4 glowColor = vec4(1.0, 0.5, 0.5, 0);

void main()
{
  vec3 normal  = normalize(v_normal);
  vec3 eye = normalize(-v_cameraPosition);
  vec3 halfVector = normalize(eye + normal);
  float NdotH = max( 0.0, dot( normal, halfVector ) );
  float shine = shininess * pow(NdotH, shininess);
  float dist = max(
    0, 
    (
      0.7*pow(v_texture.x-0.5, 2) +
      1.3*pow(v_texture.y-0.5, 2) - 0.2
    )
  );
  if (gl_FrontFacing) {
    color = texture(textures[f_cardTexture], v_texture)
      + dist*vec4( glow + glowColor * glow * shine );
  } else {
    color = texture(
      textures[f_cardTexture], vec2(
        1-v_texture.x, 
        v_texture.y
      )
    );
  }
}
)";
