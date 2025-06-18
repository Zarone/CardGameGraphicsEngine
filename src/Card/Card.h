#pragma once
#include <iostream>

class Card
{
private:
  // identities the card
  unsigned int id; 

  // identifies the instance of the card in the game
  unsigned int gameId; 
public:
  Card(unsigned int gameId);
  Card(unsigned int id, unsigned int gameId);
  unsigned int GetID();
  unsigned int GetGameID();
  void SetCardID(unsigned int cardID);
  friend std::ostream& operator<<(std::ostream& os, const Card& c);
};
