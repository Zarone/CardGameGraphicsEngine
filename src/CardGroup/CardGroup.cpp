#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CardGroup.h"

CardGroup::CardGroup(Renderer* renderer): renderer(renderer) {
}

void CardGroup::AddCard(unsigned int gameID) {
  this->cards.push_back({
    .card = Card(gameID),
    .renderData = CardRenderingData()
  });
}

void CardGroup::AddCard(unsigned int id, unsigned int gameId) {
  this->cards.push_back({
    .card = Card(id, gameId),
    .renderData = CardRenderingData()
  });
}

void CardGroup::AddCard(CardItem card) {
  this->cards.push_back(card);
}

Card CardGroup::GetCard(unsigned int index) {
  return this->cards[index].card;
}

std::vector<CardItem>* CardGroup::GetCards() {
  return &this->cards;
}

void CardGroup::MoveToGroupByGameID(unsigned int gameID, CardGroup* to, int cardID, bool supplyCardID) {
  int i = 0;
  for (auto& card :  this->cards) {
    if (card.card.GetGameID() == gameID) {
      if (supplyCardID) {
        this->cards[i].card.SetCardID(cardID);
      }
      return this->MoveToGroup(i, to);
    }

    ++i;
  }

  std::cout << "No card found with that Game ID " << gameID << std::endl;

  return;
}
