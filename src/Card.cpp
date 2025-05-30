#include "Card.h"

Card::Card(unsigned int id) 
: id(id), gameId(-1)
{
}

Card::Card(unsigned int id, unsigned int gameId) 
: id(id), gameId(gameId)
{
}

unsigned int Card::GetID() {
  return this->id;
}

unsigned int Card::GetGameID() {
  return this->gameId;
}

std::ostream& operator<<(std::ostream& os, const Card& c) {
  os << "Card: id: " << c.id << ", game id: " << c.gameId;
  return os;
}
