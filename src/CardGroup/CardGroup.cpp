#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../include/CardGroup/CardGroup.h"

CardGroup::CardGroup() {
}

void CardGroup::AddCard(unsigned int id) {
  this->cards.push_back({
    .card = Card(id),
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
