#include "CardItem.h"

std::ostream& operator<<(std::ostream& os, const CardItem& c) {
  os << "Card: " << c.card << ", renderingData: " << c.renderData;
  return os;
}
