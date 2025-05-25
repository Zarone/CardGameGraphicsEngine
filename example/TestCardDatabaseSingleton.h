#pragma once
#include "../src/CardDatabaseSingleton.h"
#include "./TestCardInfo.h"

class TestCardDatabaseSingleton : public CardDatabaseSingleton {
private:
  static TestCardInfo IDToInfo[];
  unsigned int size;
public:
  TestCardInfo* GetInfo(unsigned int id);
  TestCardDatabaseSingleton();
};
