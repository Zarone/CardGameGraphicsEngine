#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "../Card/Card.h"
#include "../Scene/SceneObject.h"
#include "../CardGroup/CardItem.h"

class CardGroup : public SceneObject
{
protected:
  // central source of data for the
  // actual card data
  std::vector<CardItem> cards = {};
  Renderer* renderer;
  virtual void MoveToGroup(int index, CardGroup* to) = 0;
public:
  CardGroup(Renderer* renderer);

  // add cards to card vector
  void AddCard(CardItem card);
  void AddCard(unsigned int id, unsigned int gameId);
  void AddCard(unsigned int id);

  Card GetCard(unsigned int index);

  std::vector<CardItem>* GetCards();

  // Setters for derived classes
  virtual void SetDirtyPosition(bool dirty) = 0;

  // Getters for derived classes
  virtual const glm::mat4 WorldSpaceToThisSpace() = 0;

  // Methods for SceneObject
  virtual void Render(Renderer* renderer) = 0;
  virtual void UpdateTick(double deltaTime) = 0;
  virtual bool CheckCollision(
    Renderer* renderer, 
    double x, 
    double y, 
    double* collisionZ, 
    CollisionInfo* info
  ) const = 0;
  virtual ClickEvent ProcessClick(CollisionInfo info) = 0;
  virtual ClickEvent ProcessPreClick(CollisionInfo info) = 0;
  virtual void ReleaseClick() = 0;
  void MoveToGroupByGameID(unsigned int gameId, CardGroup* to);
};
