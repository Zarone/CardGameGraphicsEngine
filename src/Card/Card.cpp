#include "Card.h"

Card::Card(unsigned int gameId) 
: id(-1), gameId(gameId)
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

void Card::SetCardID(unsigned int cardID) {
  this->id = cardID;
}

std::ostream& operator<<(std::ostream& os, const Card& c) {
  os << "Card: id: " << c.id << ", game id: " << c.gameId;
  return os;
}
