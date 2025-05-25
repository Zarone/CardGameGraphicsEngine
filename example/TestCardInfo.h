#pragma once
#include "../src/CardInfo.h"


class TestCardInfo : public CardInfo {
public:
  enum CardType {
    SPECIAL_CHARACTER_CARD,
    BASIC_CHARACTER_CARD
  } type;

  TestCardInfo(
    TestCardInfo::CardType,
    const std::string& imageName
  );
};
