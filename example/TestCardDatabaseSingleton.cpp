#include "./TestCardDatabaseSingleton.h"

TestCardInfo TestCardDatabaseSingleton::IDToInfo[] = {
  {
    TestCardInfo::CardType::SPECIAL_CHARACTER_CARD,
    "card1"
  },
  {
    TestCardInfo::CardType::SPECIAL_CHARACTER_CARD,
    "card2"
  },
  {
    TestCardInfo::CardType::SPECIAL_CHARACTER_CARD,
    "card3"
  },
  {
    TestCardInfo::CardType::BASIC_CHARACTER_CARD,
    "card4"
  },
  {
    TestCardInfo::CardType::EVENT,
    "card5"
  },
  {
    TestCardInfo::CardType::ACTION,
    "card6"
  },
  {
    TestCardInfo::CardType::BASIC_CHARACTER_CARD,
    "card7"
  },
};

TestCardDatabaseSingleton::TestCardDatabaseSingleton() {
  this->size = sizeof(TestCardDatabaseSingleton::IDToInfo)/sizeof(TestCardInfo);
}

TestCardInfo* TestCardDatabaseSingleton::GetInfo(unsigned int id) {
  if (id >= size || id < 0) return nullptr;
  else {
    return &(IDToInfo[id]);
  }
}
