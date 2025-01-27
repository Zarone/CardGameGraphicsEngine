#pragma once

#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>

#include "Card.h"
#include "CardGroupRenderer.h"
#include "SceneObject.h"
#include "../include/CardItem.h"
 
enum RendererType {
  FullView,
  Fanned,
  Stacked
};

class CardGroup : public SceneObject
{
protected:
  std::vector<CardItem> cards = {};
public:
  CardGroup();

  void AddCard(CardItem card);

  void AddCard(unsigned int id);

  Card GetCard(unsigned int index);

  std::vector<CardItem>* GetCards();

  virtual bool CheckCollision(
    Renderer* renderer, 
    double x, 
    double y, 
    double* collisionZ, 
    CollisionInfo* info
  ) const = 0;
  virtual void Render(Renderer* renderer) = 0;
  virtual void UpdateTick(double deltaTime) = 0;
  virtual ClickEvent ProcessClick(CollisionInfo info) = 0;
  virtual ClickEvent ProcessPreClick(CollisionInfo info) = 0;
  virtual void ReleaseClick() = 0;
  virtual void SetNumHighlightedCards(int num) = 0;
  virtual void SetDirtyPosition(bool dirty) = 0;
  virtual const glm::mat4 WorldSpaceToThisSpace() = 0;
};
