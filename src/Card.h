#pragma once
#include <iostream>

class Card
{
private:
  unsigned int id;
public:
  Card(unsigned int id);
  unsigned int GetID();
  friend std::ostream& operator<<(std::ostream& os, const Card& c);
};
