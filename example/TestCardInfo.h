#pragma once
#include "../src/Card/CardInfo.h"


class TestCardInfo : public CardInfo {
public:
  enum CardType {
    SPECIAL_CHARACTER_CARD,
    BASIC_CHARACTER_CARD,
    EVENT,
    ACTION
  } type;

  TestCardInfo(
    TestCardInfo::CardType,
    const std::string& imageName
  );
};
