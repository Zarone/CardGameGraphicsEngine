#include "CardGroup.h"
#include "FannedCardGroup.h"

class StackedCardGroup : public CardGroup
{
private:
  glm::mat4 transform;

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
public:
  StackedCardGroup(
    Renderer* renderer,
    glm::vec3 position, 
    float rotationX, 
    bool zFlipped
  );

  void MoveToGroup(int index, FannedCardGroup* to);

  const glm::mat4 WorldSpaceToThisSpace();

  void Render(Renderer* renderer); 

  bool CheckCollision(
    Renderer* renderer, 
    double x, 
    double y, 
    double* collisionZ,
    CollisionInfo* collisioInfo
  ) const;

};
