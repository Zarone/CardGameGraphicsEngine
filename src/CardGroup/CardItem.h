#pragma once
#include "../Card/Card.h"
#include "CardRenderingData.h"

typedef struct CardItem {
  Card card;
  CardRenderingData renderData;
  friend std::ostream& operator<<(std::ostream& os, const CardItem& c);
} CardItem;

