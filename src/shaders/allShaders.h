#pragma once
#include <string>

enum ShaderID {
  ButtonShader,
  CardShader,
  HighlightCardShader,
  SelectedCardShader,
  BasicShader
};

namespace myShaders {
  extern std::string basicVertex;
  extern std::string basicFragment;
  extern std::string cardVertex;
  extern std::string cardFragment;
  extern std::string textureVertex;
  extern std::string textureFragment;
  extern std::string highlightedCardVertex;
  extern std::string highlightedCardFragment;
  extern std::string selectedCardFragment;
}
