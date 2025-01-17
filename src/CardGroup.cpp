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
  float scaleX, 
  float scaleY, 
  bool zFlipped
) 
: 
  textureMap(textureMap), 
  zFlipped(zFlipped), 
  dirty(true), 
  cardShader(myShaders::cardVertex, myShaders::cardFragment)
{
  this->transform = glm::mat4(1.0f); // setup to identity
  this->transform = glm::rotate(this->transform, rotationX, glm::vec3(1.0f, 0.0f, 0.0f));
  this->transform = glm::rotate(this->transform, rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));
  this->transform = glm::scale(this->transform, glm::vec3(scaleX, scaleY, zFlipped ? -1.0f : 1.0f));

  this->groupVao = VertexArray();
  this->groupVao.Bind();

  this->staticBuffer = VertexBuffer(CardRenderingData::cardPositions, 5*4*sizeof(float));
  this->staticBufferLayout = MemoryLayout();
  this->staticBufferLayout.AddMemoryElement(GL_FLOAT, 3); // position
  this->staticBufferLayout.AddMemoryElement(GL_FLOAT, 2); // texture uv coordinates
  this->groupVao.AddBuffer(this->staticBuffer, this->staticBufferLayout);

  this->transformBuffer = VertexBuffer(NULL, estimatedMax*3*sizeof(float), true);
  this->transformBufferLayout = MemoryLayout();
  this->transformBufferLayout.AddMemoryElement(GL_FLOAT, 3, true); // position relative group
  this->transformBufferLayout.AddMemoryElement(GL_FLOAT, 1, true); // rotation
  this->groupVao.AddBuffer(this->transformBuffer, this->transformBufferLayout);

  this->textureIDBuffer = VertexBuffer(NULL, estimatedMax*3*sizeof(float), true);
  this->textureIDBufferLayout = MemoryLayout();
  this->textureIDBufferLayout.AddMemoryElement(GL_INT, 1, true); // texture slot
  this->groupVao.AddBuffer(this->textureIDBuffer, this->textureIDBufferLayout);

  this->indexBuffer = IndexBuffer(CardRenderingData::cardIndices, 2);
}

void CardGroup::PrepareTextures() {
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
}

void CardGroup::Render(
  unsigned int maxBindableTextures, 
  glm::mat4& projMatrix, 
  glm::mat4& camMatrix
) {
  int size = this->cards.size();

  if (this->dirty) {
    // update buffer for relative position and rotation
    
    // 3 for positions plus 1 for rotation
    int vertexSize = 3+1;

    float buffer[vertexSize*size];
    for (int i = 0; i < size; i+=vertexSize) {
      buffer[i] = (float)i/10.0f;
      buffer[i+1] = 0.0f;
      buffer[i+2] = (float)i/10.0f;
      buffer[i+3] = (float)i/10.0f;
    }

    this->transformBuffer.RewriteData(buffer, size, true);
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
  //std::vector<int> textureUnits(maxBindableTextures);
  //std::iota(textureUnits.begin(), textureUnits.end(), 0); // {0, 1, 2, ...}
  //cardShader.SetUniform1iv("textures", maxBindableTextures, textureUnits.data());

  if (this->zFlipped) {
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
      this->transformBuffer.OverwriteData(
        buffer, 
        i*sizeof(GLint)*vertexSize, 
        batchSize*sizeof(GLint)*vertexSize
      );

      GLCall(glDrawElementsInstanced(
        GL_TRIANGLES, 
        6, 
        GL_UNSIGNED_INT, 
        (const void*) (i*this->transformBufferLayout.GetStride()),
        fmin(maxBindableTextures, size-i)
      ));

      i += maxBindableTextures;
      this->textureMap->ResetIndexing();
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
