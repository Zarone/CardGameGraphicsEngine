#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "../include/CardGroup.h"
#include "../include/ErrorHandling.h"
#include "../include/shaders/allShaders.h"

const unsigned int CardGroup::estimatedMax = 20;

CardGroup::CardGroup(
  TextureMap* textureMap, 
  glm::vec3 position, 
  float rotationX, 
  float rotationZ, 
  float width, 
  bool zFlipped
) 
: 
textureMap(textureMap), 
zFlipped(zFlipped), 
dirtyDisplay(true), 
dirtyPosition(true), 
cardShader(myShaders::cardVertex, myShaders::cardFragment),
width(width),
lastCursorX(0),
lastCursorY(0),
wasInsideBoundary(false),
lastClosestIndex(-1)
{
  this->transform = glm::mat4(1.0f); // setup to identity
  this->transform = glm::translate(this->transform, position);
  this->transform = glm::translate(this->transform, glm::vec3(-width/2.0f+0.5f, 0.0f, 0.0f));
  this->transform = glm::rotate(this->transform, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
  this->transform = glm::rotate(this->transform, glm::radians(rotationZ), glm::vec3(0.0f, 0.0f, 1.0f));
  this->transform = glm::scale(this->transform, glm::vec3(1.0f, 1.0f, zFlipped ? -1.0f : 1.0f));

  this->groupVao = VertexArray();
  this->groupVao.Bind();

  this->staticBuffer = VertexBuffer(CardRenderingData::cardPositions, 5*4*sizeof(float));
  this->staticBufferLayout = MemoryLayout();
  this->staticBufferLayout.AddMemoryElement(GL_FLOAT, 3); // position
  this->staticBufferLayout.AddMemoryElement(GL_FLOAT, 2); // texture uv coordinates
  this->groupVao.AddBuffer(this->staticBuffer, this->staticBufferLayout);

  this->transformBuffer = VertexBuffer(NULL, estimatedMax*4*sizeof(float), true);
  this->transformBufferLayout = MemoryLayout();
  this->transformBufferLayout.AddMemoryElement(GL_FLOAT, 3, true); // position relative group
  this->transformBufferLayout.AddMemoryElement(GL_FLOAT, 1, true); // rotation
  this->transformEndAttribID = this->groupVao.AddBuffer(this->transformBuffer, this->transformBufferLayout);

  this->textureIDBuffer = VertexBuffer(NULL, estimatedMax*1*sizeof(GLint), true);
  this->textureIDBufferLayout = MemoryLayout();
  this->textureIDBufferLayout.AddMemoryElement(GL_INT, 1, true); // texture slot
  this->textureEndAttribID = this->groupVao.AddBuffer(this->textureIDBuffer, this->textureIDBufferLayout);

  this->indexBuffer = IndexBuffer(CardRenderingData::cardIndices, 2);
}

void CardGroup::PrepareTextures() {
  int size = this->cards.size();

  if (zFlipped) {
    this->textureMap->SetupBack();
  } else {
    for (CardItem& cardItem : this->cards) {
      this->textureMap->SetupCard(cardItem.card.GetID());
    }
  }

  this->textureIDBuffer.RewriteData(NULL, size*sizeof(GLint), true);
}

void CardGroup::DrawElements(int size) {
  GLCall(glDrawElementsInstanced(
    GL_TRIANGLES, 
    6, 
    GL_UNSIGNED_INT, 
    nullptr,
    size 
  ));
}

void CardGroup::Render(
  Renderer& renderer,
  const RenderData& renderData
) {
  glm::mat4& projMatrix = renderer.projMatrix;
  glm::mat4& camMatrix = renderer.cameraMatrix;
  int maxBindableTextures = renderer.maxBindableTextures;

  bool insideHandBoundary = false;
  bool mouseMovedInBoundary = false;

  // I store this because I use it in
  // determining closest card
  double projectedLeftBoundary = -1.0f;
  double xScale = -1.0f;

  const float verticalMargin = 0.15f;
  const float horizontalMargin = 0.15f;

  // if we need to know boundaries
  if (renderData.isHand) {
    // check if cursor is near the card group

    // if the cursor moved
    if (
      renderer.InsideWindowBounds(renderData.cursorX, renderData.cursorY) &&
      (
        (int)renderData.cursorY != lastCursorY || 
        (int)renderData.cursorX != lastCursorX
      )
    ) {

      // project top bound to screen
      glm::vec4 topLeft = glm::vec4(
        -0.5f-horizontalMargin, 
        0.5f*CardRenderingData::cardHeightRatio+verticalMargin, 
        0.0f, 
        1.0f
      );
      glm::vec4 topLeftScreenSpace = projMatrix * camMatrix * this->transform * topLeft;
      glm::vec2 projectedTopLeft = renderer.GetScreenPositionFromCamera(topLeftScreenSpace);
      
      // project bottom bound to screen
      glm::vec4 bottomRight = glm::vec4(
        -0.5f+width+horizontalMargin, 
        -0.5f*CardRenderingData::cardHeightRatio-verticalMargin, 
        0.0f, 
        1.0f
      );
      glm::vec4 bottomRightScreenSpace = projMatrix * camMatrix * this->transform * bottomRight;
      glm::vec2 projectedBottomRight = renderer.GetScreenPositionFromCamera(bottomRightScreenSpace);
      
      xScale = (projectedBottomRight.x - projectedTopLeft.x)/(this->width+2*horizontalMargin);
      projectedLeftBoundary = projectedTopLeft.x+horizontalMargin*xScale;

      // see if cursor is below top left boundary and 
      // above bottom right boundary
      if (
        renderData.cursorX > projectedTopLeft.x && renderData.cursorY > projectedTopLeft.y &&
        renderData.cursorX < projectedBottomRight.x && renderData.cursorY < projectedBottomRight.y 
      ) {
        mouseMovedInBoundary = true;
        insideHandBoundary = true;
      }
    } else {
      insideHandBoundary = this->wasInsideBoundary;
    }
  }

  if (this->wasInsideBoundary != insideHandBoundary) this->dirtyPosition = true;

  int size = this->cards.size();

  // prevent division by 0 error
  float xGap = size == 1 ? 0 : (this->width-1)/(size-1);
  float zGap = 1.0f/50.0f;

  // check is the closest card changed.
  // if it didn't, we don't need to rerender
  if (mouseMovedInBoundary) {
    // find closest card
    int closestIndex = 0;
    double thisX = projectedLeftBoundary + 0.5f*xScale;
    double smallestDistance = abs(thisX-renderData.cursorX);
    for (int i = 1; i < size; ++i) {
      thisX += xGap*xScale;
      double distance = abs(thisX - renderData.cursorX);
      if (distance < smallestDistance) {
        smallestDistance = distance;
        closestIndex = i;
      }
    }

    if (closestIndex != this->lastClosestIndex) {
      this->lastClosestIndex = closestIndex;
      this->dirtyPosition = true;
    }
  }

  if (this->dirtyPosition) {
    float rotationPerCard = renderData.isHand ? 1.0f/20.0f : 0;

    // if we need to display the cards
    // dynamically based on cursor position
    if (renderData.isHand && insideHandBoundary) {

      const double whitespace = 0.25f;
      
      int leftSize = this->lastClosestIndex;
      int rightSize = size-this->lastClosestIndex-1;
      double leftWidth = (this->width-whitespace-1)*((double)leftSize/(size-1));
      double rightWidth = (this->width-whitespace-1)*((double)rightSize/(size-1));
      if (leftWidth < 1.25f) leftWidth = 1.25f;
      if (rightWidth < 1.25f) rightWidth = 1.25f;
      double leftGap = fmax(0, leftSize == 1 ? 0 : (leftWidth-1.0f)/(leftSize-1.0f));
      double rightGap = fmax(0, rightSize == 1 ? 0 : (rightWidth-1.0f)/(rightSize-1.0f));

      //std::cout << std::endl;
      //std::cout << "Closest Index: " << this->lastClosestIndex << std::endl;
      //std::cout << "Left Size: " << leftSize << std::endl;
      //std::cout << "Right Size: " << rightSize << std::endl;
      //std::cout << "Left Width: " << leftWidth << std::endl;
      //std::cout << "Right Width: " << rightWidth << std::endl;
      //std::cout << "Left Gap: " << leftGap << std::endl;
      //std::cout << "Right Gap: " << rightGap << std::endl;

      // setup buffer info that depends
      // on left/right divide
      for (int i = 0; i < this->lastClosestIndex; i++) {
        int cardIndex = i;
        CardRenderingData& thisCard = cards[cardIndex].renderData;

        thisCard.SetActualTransform(
          glm::vec3(
            (float)cardIndex*leftGap,
            0.0f,
            (float)cardIndex*zGap
          ),
          (float)(cardIndex-(float)(size-1)/2.0f)*rotationPerCard
        );
      }

      CardRenderingData& closestCard = cards[this->lastClosestIndex].renderData;
      closestCard.SetActualTransform(
        glm::vec3(this->lastClosestIndex*xGap, 0.0f, size*zGap),
        0.0f
      );

      for (int i = (this->lastClosestIndex+1); i < size; i++) {
        int cardIndex = i;
        CardRenderingData& thisCard = cards[cardIndex].renderData;

        thisCard.SetActualTransform(
          glm::vec3(
            (float)(this->lastClosestIndex*xGap + 0.5f+whitespace+ (cardIndex-this->lastClosestIndex)*rightGap),
            0.0f,
            (float)cardIndex*zGap
          ),
          (float)(cardIndex-(float)(size-1)/2.0f)*rotationPerCard
        );
      }
    } else {
      for (int i = 0; i < size; i++) {
        int cardIndex = i;

        CardRenderingData& thisCard = cards[cardIndex].renderData;

        thisCard.SetActualTransform(
          glm::vec3(
            (float)cardIndex*xGap,
            0.0f,
            (float)cardIndex*zGap
          ),
          (float)(cardIndex-(float)(size-1)/2.0f)*rotationPerCard
        );
      }
    }
  }

  // 3 for positions plus 1 for rotation
  const int transformVertexSize = 3+1;

  if (this->dirtyDisplay) {

    // update buffer for relative position and rotation
    float buffer[transformVertexSize*size];

    // load display info from cards into buffer
    for (int i = 0; i < size*transformVertexSize; i+=transformVertexSize) {
      int cardIndex = i/transformVertexSize;
      CardRenderingData& thisCard = cards[cardIndex].renderData;

      buffer[i] = thisCard.displayedPosition.x;
      buffer[i+1] = thisCard.displayedPosition.y;
      buffer[i+2] = thisCard.displayedPosition.z;
      buffer[i+3] = thisCard.displayedRotationZ;
    }


    this->transformBuffer.RewriteData(buffer, size*transformVertexSize*sizeof(GLfloat), true);
  }

  this->groupVao.Bind();
  this->indexBuffer.Bind();
  this->transformBuffer.Bind();
  this->cardShader.Bind();
  cardShader.SetUniform4fv("projMatrix", false, glm::value_ptr(projMatrix));
  cardShader.SetUniform4fv("cameraMatrix", false, glm::value_ptr(camMatrix));
  cardShader.SetUniform4fv("modelMatrix", false, glm::value_ptr(this->transform));

  int i = 0;

  // Pass texture units as an array to the shader.
  std::vector<int> textureUnits(maxBindableTextures);
  int mapSize = this->textureMap->Size();
  for (int i = 0; i < maxBindableTextures; ++i) {
    if (i < mapSize) {
      textureUnits[i] = i;
    } else {
      textureUnits[i] = 0;
    }
  }
  cardShader.SetUniform1iv("textures", maxBindableTextures, textureUnits.data());

  if (this->zFlipped) {
    this->textureMap->RequestBind(maxBindableTextures, "back");
    this->DrawElements(size);
  } else {
    // just the texture slot id
    int vertexSize = 1;

    int buffer[vertexSize*size];

    while (i < size) {
      int batchSize = fmin(maxBindableTextures, size-i);

      // bind all textures about to be used
      for (int j = i; j < i+batchSize; ++j) {
        unsigned int cardID = this->cards[j].card.GetID();

        this->textureMap->RequestBind(maxBindableTextures, cardID);
        
        // update texture buffer using newly bound addresses
        buffer[j] = this->textureMap->GetSlotOf(cardID);
      }

      // write data from buffer to gpu
      this->textureIDBuffer.OverwriteData(
        buffer+i*vertexSize, 
        i*sizeof(GLint)*vertexSize, 
        batchSize*sizeof(GLint)*vertexSize
      );


      // this changes the offset within the
      // instanced buffers so that they are read
      // from the right offset.
      this->textureIDBuffer.OverwriteAttrib(
        this->textureEndAttribID, 
        1, 
        GL_UNSIGNED_INT, 
        sizeof(GLuint)*vertexSize, 
        (const void*)(i*sizeof(GLuint))
      );

      this->transformBuffer.OverwriteAttrib(
        this->transformEndAttribID-1,
        3,
        GL_FLOAT,
        sizeof(GLfloat)*transformVertexSize, 
        (const void*)(i*sizeof(GLfloat)*transformVertexSize)
      );

      this->transformBuffer.OverwriteAttrib(
        this->transformEndAttribID,
        1,
        GL_FLOAT,
        sizeof(GLfloat)*transformVertexSize, 
        (const void*)(i*sizeof(GLfloat)*transformVertexSize+3*sizeof(GLfloat))
      );

      this->DrawElements(batchSize);

      i += maxBindableTextures;
    }
  }

  this->dirtyPosition = false;
  this->dirtyDisplay = false;

  if (renderData.isHand) {
    this->lastCursorX = (int)renderData.cursorX;
    this->lastCursorY = (int)renderData.cursorY;
    this->wasInsideBoundary = insideHandBoundary;
  }
}

void CardGroup::AddCard(unsigned int id) {
  this->cards.push_back({
    .card = Card(id),
    .renderData = CardRenderingData()
  });
}

void CardGroup::UpdateTick(double deltaTime) {
  bool anythingUpdated = false;
  for (auto& card : this->cards) {
    anythingUpdated = card.renderData.UpdateDisplayed(deltaTime) || anythingUpdated;
  }
  this->dirtyDisplay = true;
}
