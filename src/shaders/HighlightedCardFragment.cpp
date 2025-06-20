#include "allShaders.h"

std::string myShaders::highlightedCardFragment = R"(
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_texture;
in vec3 v_normal;
in vec3 v_cameraPosition;
flat in int f_cardTexture;

uniform sampler2D textures[16];

float shininess = 8.0;
float glow = 1.0;
vec4 glowColor = vec4(1.0, 0.5, 0.5, 1.0);

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

  vec2 uv = v_texture;

  if (superEllipse(uv) > 0.9) {
    discard;
  }

  uv = uv / scale - (1-scale)*0.5;

  if (superEllipse(uv) > 0.9) {
    color = dist*vec4( glowColor * glow * shine );
    return;
  }

  vec4 texColor = (gl_FrontFacing)
      ? texture(textures[f_cardTexture], v_texture)
        + dist*vec4( glow + glowColor * glow * shine )
      : texture(textures[f_cardTexture], vec2(1.0 - uv.x, uv.y));
  color = texColor;

}
)";
