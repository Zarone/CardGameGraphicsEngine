#pragma once

#include "../../include/SceneObject.h"
#include "../../include/CardGroup/CardItem.h"
#include "../../include/HelperOpenGL/VertexArray.h"
#include "../../include/HelperOpenGL/IndexBuffer.h"

typedef struct CardTransformVertex {
  float x;
  float y;
  float z;
  float rotationZ;
  float scaleXY;
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

class CardGroupRenderer : public SceneObject
{
protected:
  // Makes sure this class can access the
  // cards. This variable is set in CardGroup
  // which contains this object.
  std::vector<CardItem>* cardsPointer = nullptr;

  // Information for rendering to OpenGL
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

  Renderer* renderer;
  
  // keeps track of whether or not 
  // we need to update position info 
  // before the next render.
  bool dirtyDisplay; 
  bool dirtyPosition;

  // max number of cards that can be in
  // this group
  const static unsigned int estimatedMax; 

  /*
   * @brief This is a helper function for loading
   * the position data into the given buffer.
   *
   */
  void LoadPositions(
    float* buffer,
    Shader* cardShader,
    int offset,
    int numPositions,
    int size,
    bool reverse = false
  );

  /*
   * @brief This loads the data from cards into
   * the GPU. It makes sure to bind the correct
   * textures beforehand.
   *
   */
  void BindAndDrawAllFrontFaces(
    Renderer* renderer,
    Shader* shader,
    int maxBindableTextures,
    int offset,
    int groupSize,
    int totalSize,
    bool zFlipped
  );

  void GroupPositionToScreen(
    Renderer* renderer, 
    glm::vec4& src, 
    glm::vec2& dest
  ) const;

  void GroupPositionTo3DScreen(
    Renderer* renderer, 
    glm::vec4& src, 
    glm::vec3& dest
  ) const;

  bool IsInsideScreenRectangle(
    Renderer* renderer,
    double x, 
    double y, 
    glm::vec4 topLeft,
    glm::vec4 topRight,
    glm::vec4 bottomLeft,
    glm::vec4 bottomRight,
    double width,
    double* leftBoundary,
    double* zAtCursor,
    double* xScale
  ) const;
public:
  int highlightedCards = 0;

  glm::mat4 transform;

  CardGroupRenderer(Renderer* renderer, bool zFlipped);

  /*
   * @brief This is just a setter for cards.
   *
   */
  void SetCardsPointer(std::vector<CardItem> *);

  /*
  *
  * @brief Must be called after cards are added to group.
  * Makes sure all textures are loaded into the texture map.
  *
  */
  void PrepareTextures(TextureMap* textureMap, int start, int end);

  /*
   * @brief This just makes the OpenGL call to use instanced
   * drawing.
   *
   *
   */
  void DrawElements(int size);

  /*
   * @brief This is just a setter for dirtyPosition
   *
   */
  void SetDirtyPosition(bool dirty);

  void UpdateTick(double deltaTime);

  virtual const glm::mat4 WorldSpaceToThisSpace() = 0;

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

  virtual ClickEvent ProcessClick(CollisionInfo info) {return {};}

  virtual void ReleaseClick() {};

  // Game state should perform operations on the card group,
  // so that it can correctly manipulate data. So these can just
  // be blank unless I need them for something else.
  ClickEvent ProcessPreClick(CollisionInfo info) {return {};};
};
