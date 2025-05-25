#include "Card.h"

Card::Card(unsigned int id) 
: id(id)
{
}

unsigned int Card::GetID() {
  return this->id;
}

std::ostream& operator<<(std::ostream& os, const Card& c) {
  os << "Card: id: " << c.id;
  return os;
}
