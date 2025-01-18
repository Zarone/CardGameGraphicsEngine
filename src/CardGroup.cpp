#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <numeric>
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
dirty(true), 
cardShader(myShaders::cardVertex, myShaders::cardFragment),
width(width)
{
  this->transform = glm::mat4(1.0f); // setup to identity
  this->transform = glm::translate(this->transform, position);
  this->transform = glm::translate(this->transform, glm::vec3(-width/2.0f+0.5f, 0.0f, 0.0f));
  this->transform = glm::rotate(this->transform, rotationX, glm::vec3(1.0f, 0.0f, 0.0f));
  this->transform = glm::rotate(this->transform, rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));
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
    std::cout << "Setting up cards..." << std::endl;
    for (CardItem& cardItem : this->cards) {
      std::cout << "Setting up card: " << cardItem.card << std::endl;
      Card& card = cardItem.card;
      this->textureMap->SetupCard(card.GetID());
    }
  }

  this->textureIDBuffer.RewriteData(NULL, size*sizeof(GLint), true);

}

void CardGroup::Render(
  unsigned int maxBindableTextures, 
  glm::mat4& projMatrix, 
  glm::mat4& camMatrix
) {
  int size = this->cards.size();

  // 3 for positions plus 1 for rotation
  int transformVertexSize = 3+1;

  if (this->dirty) {
    // update buffer for relative position and rotation
    
    float buffer[transformVertexSize*size];
    for (int i = 0; i < size*transformVertexSize; i+=transformVertexSize) {
      int cardIndex = i/transformVertexSize;

      float xGap = (this->width-1)/(size-1);
      std::cout << "Gap = " << xGap << std::endl;

      // I split this for divide by 1 error
      if (size == 1) {
        buffer[i] = (float)0.0f;
      } else {
        std::cout << "x[" << cardIndex << "] = " << cardIndex*xGap << std::endl;
        buffer[i] = (float)cardIndex*xGap;
      }

      buffer[i+1] = 0.0f;
      buffer[i+2] = (float)i/200.0f;
      buffer[i+3] = (float)i/10.0f;
    }
    PrintVector(std::cout, std::vector<float>(buffer, buffer + sizeof(buffer)/sizeof(buffer[0])));

    this->transformBuffer.RewriteData(buffer, size*transformVertexSize*sizeof(GLfloat), true);
  }

  int i = 0;

  this->groupVao.Bind();
  this->indexBuffer.Bind();
  this->transformBuffer.Bind();
  this->cardShader.Bind();
  cardShader.SetUniform4fv("projMatrix", false, glm::value_ptr(projMatrix));
  cardShader.SetUniform4fv("cameraMatrix", false, glm::value_ptr(camMatrix));
  cardShader.SetUniform4fv("modelMatrix", false, glm::value_ptr(this->transform));

  // Pass texture units as an array to the shader
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
    GLCall(glDrawElementsInstanced(
      GL_TRIANGLES, 
      6, 
      GL_UNSIGNED_INT, 
      (const void*) 0,
      size
    ));
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

      this->groupVao.Bind();

      this->textureIDBuffer.Bind();
      GLCall(glVertexAttribPointer(
        this->textureEndAttribID, 
        1, 
        GL_UNSIGNED_INT, 
        GL_FALSE, 
        sizeof(GLuint)*vertexSize, 
        (const void*)(i*sizeof(GLuint))
      ));

      this->transformBuffer.Bind();
      GLCall(glVertexAttribPointer(
        this->transformEndAttribID-1, 
        3, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(GLfloat)*transformVertexSize, 
        (const void*)(i*sizeof(GLfloat)*transformVertexSize)
      ));
      GLCall(glVertexAttribPointer(
        this->transformEndAttribID, 
        1, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(GLfloat)*transformVertexSize, 
        (const void*)(i*sizeof(GLfloat)*transformVertexSize+3*sizeof(GLfloat))
      ));

      GLCall(glDrawElementsInstanced(
        GL_TRIANGLES, 
        6, 
        GL_UNSIGNED_INT, 
        nullptr,
        batchSize
      ));

      i += maxBindableTextures;
    }
  }

  this->dirty = false;
}

void CardGroup::AddCard(unsigned int id) {
  this->cards.push_back({
    Card(id),
    CardRenderingData()
  });
}
