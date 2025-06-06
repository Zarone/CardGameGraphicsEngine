#pragma once
#include "CardInfo.h"

class CardDatabaseSingleton
{
public:
  virtual CardInfo* GetInfo(unsigned int id) = 0;
};
