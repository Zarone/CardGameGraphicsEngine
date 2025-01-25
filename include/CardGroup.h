#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "CardRenderingData.h"
#include "Card.h"
#include "Renderer.h"
#include "SceneObject.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "TextureMap.h"
#include "SimplePlane.h"
#include "ErrorHandling.h"
#include "../include/shaders/allShaders.h"

typedef struct CardItem {
  Card card;
  CardRenderingData renderData;
} CardItem;
 
typedef struct CardTransformVertex {
  float x;
  float y;
  float z;
  float rotationZ;
} CardTransformVertex;

typedef struct CardVertex {
  float x;
  float y;
  float z;
  float u;
  float v;
} CardVertex;

typedef struct TextureVertex {
  unsigned int index;
} TextureVertex;

class CardGroup : public SceneObject
{
protected:
  const static unsigned int estimatedMax;

  std::vector<CardItem> cards = {};

  // if this is true, only the back 
  // of the cards is shown
  bool zFlipped; 
public:
  int highlightedCards = 0;

  CardGroup(
    bool zFlipped
  );

  /*
  *
  * @brief Must be called after cards are added to group
  *
  */
  void PrepareTextures(TextureMap* textureMap);

  void AddCard(unsigned int id);

  Card GetCard(unsigned int index);

  std::vector<CardItem>* GetCards();

  virtual void UpdateTick(double deltaTime) = 0;

  virtual void Render(Renderer* renderer) = 0;

  /*
  *
  * @brief Checks for collision between this card group
  * and the cursor
  *
  */
  virtual bool CheckCollision(
    Renderer* renderer, 
    double x, 
    double y, 
    double* collisionZ,
    CollisionInfo* collisioInfo
  ) const = 0;

  // Game state should perform operations on the card group,
  // so that it can correctly manipulate data. So these can just
  // be blank unless I need them for something else.
  ClickEvent ProcessClick(CollisionInfo info) {return {};}
  ClickEvent ProcessPreClick(CollisionInfo info) {return {};};
  void ReleaseClick() {};
};
