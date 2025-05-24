#pragma once

#include <glm/glm.hpp>

#include "../../include/HelperOpenGL/VertexArray.h"
#include "../../include/HelperOpenGL/IndexBuffer.h"
#include "../../include/Renderer.h"
#include "../../include/SceneObject.h"
#include "../../include/Material.h"

class SimpleRenderObject : public SceneObject
{
private:
  const int vertexSize = 5; // 3 for position, 2 for texture
  VertexArray vArray;
  VertexBuffer vBuffer;
  IndexBuffer iBuffer;
  int triangleCount;
protected:
  void LoadIntoGPU();
  virtual int GetVertexCount() = 0;
  virtual void FillData(float* vertexData, unsigned int* indexData) = 0;
  Material material;
  bool perspective;
  glm::mat4 transform; // Store the transform matrix
public:
  SimpleRenderObject(
    glm::mat4 transform,
    Material material
  );

  virtual void SetTransform(glm::mat4* transform);
  void TogglePerspective(bool perspective);

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
