#pragma once

#include <glm/glm.hpp>

#include "../include/VertexArray.h"
#include "../include/IndexBuffer.h"
#include "../include/Shader.h"
#include "../include/Renderer.h"
#include "../include/SceneObject.h"

class SimpleRenderObject : public SceneObject
{
private:
  const int vertexSize = 5; // 3 for position, 2 for texture
  VertexArray vArray;
  VertexBuffer vBuffer;
  IndexBuffer iBuffer;
  Shader shader;
  int triangleCount;
protected:
  void LoadIntoGPU();
  virtual int GetVertexCount() = 0;
  virtual void FillData(float* vertexData, unsigned int* indexData) = 0;
  glm::vec4 color;
public:
  SimpleRenderObject(
    const std::string& vertexShader, 
    const std::string& fragmentShader,
    glm::mat4 transform,
    glm::vec4 color
  );

  virtual void SetTransform(glm::mat4* transform);
  void Render(Renderer* renderer);
  void UpdateTick(double deltaTime);
  virtual bool CheckCollision(
    Renderer* renderer, 
    double x, 
    double y, 
    double* collisionZ, 
    CollisionInfo* info
  ) const;
  virtual ClickEvent ProcessClick(CollisionInfo info);
  virtual ClickEvent ProcessPreClick(CollisionInfo info);
  virtual void ReleaseClick();
};
