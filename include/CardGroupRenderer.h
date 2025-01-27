#pragma once

#include "SceneObject.h"
#include "../include/CardItem.h"
#include "VertexArray.h"
#include "IndexBuffer.h"

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

class CardGroupRenderer
{
protected:
  std::vector<CardItem>* cardsPointer = {};

  VertexArray groupVao;
  VertexBuffer staticBuffer;
  IndexBuffer indexBuffer;
  MemoryLayout staticBufferLayout;
  VertexBuffer transformBuffer;
  MemoryLayout transformBufferLayout;
  unsigned int transformEndAttribID;
  VertexBuffer textureIDBuffer;
  MemoryLayout textureIDBufferLayout;
  unsigned int textureEndAttribID;

  // if this is true, only the back 
  // of the cards is shown
  bool zFlipped; 
  
  // keeps track of whether or not 
  // we need to update position info 
  // before the next render.
  bool dirtyDisplay; 
  bool dirtyPosition;

  // max number of cards that can be in
  // this group
  const static unsigned int estimatedMax; 

  void LoadPositions(
    float* buffer,
    Shader* cardShader,
    int offset,
    int numPositions,
    int size,
    bool reverse = false
  );

  void BindAndDrawAllFrontFaces(
    Renderer* renderer,
    Shader* shader,
    int maxBindableTextures,
    int offset,
    int groupSize,
    int totalSize,
    bool zFlipped
  );

public:
  int highlightedCards = 0;

  CardGroupRenderer(bool zFlipped);

  void SetCardsPointer(std::vector<CardItem> *);

  /*
  *
  * @brief Must be called after cards are added to group
  *
  */
  void PrepareTextures(TextureMap* textureMap, int start, int end);

  void DrawElements(int size);

  void SetDirtyPosition(bool dirty);

  virtual const glm::mat4 WorldSpaceToThisSpace() = 0;

  virtual void Render(Renderer* renderer) = 0;

  void UpdateTick(double deltaTime);

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
