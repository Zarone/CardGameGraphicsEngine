#include <glm/gtc/type_ptr.hpp>
#include "SimpleRenderObject.h"
#include "../Helper/ErrorHandling.h"

SimpleRenderObject::SimpleRenderObject(
  glm::mat4 transform,
  Material material
):
  material(material),
  perspective(true),
  transform(transform)
{
  this->SetTransform(&transform);
}

glm::mat4 SimpleRenderObject::GetTransform() const {
  return this->transform;
}

void SimpleRenderObject::LoadIntoGPU() {
  int count = this->GetVertexCount();
  int triangleCount = count - 2;
  this->triangleCount = triangleCount;
  
  float vertexData[count*vertexSize];
  unsigned int indexData[triangleCount*3];

  this->FillData(vertexData, indexData);

  this->vArray = VertexArray();
  this->vArray.Bind();

  this->vBuffer = VertexBuffer(vertexData, count*vertexSize*sizeof(float));
  MemoryLayout vertexLayout;
  vertexLayout.AddMemoryElement(GL_FLOAT, 3); // position 
  vertexLayout.AddMemoryElement(GL_FLOAT, 2); // texture
  this->vArray.AddBuffer(this->vBuffer, vertexLayout);

  this->iBuffer = IndexBuffer(indexData, triangleCount);
}

void SimpleRenderObject::SetTransform(glm::mat4* transform) {
  this->transform = *transform;
}

void SimpleRenderObject::TogglePerspective(bool perspective) {
  this->perspective = perspective;
}

void SimpleRenderObject::Render(Renderer* renderer) {
  this->material.shader->Bind();
  this->material.shader->SetUniform4fv("modelMatrix", false, glm::value_ptr(this->transform));

  this->vArray.Bind();
  this->vBuffer.Bind();
  this->iBuffer.Bind();
  this->material.shader->Bind();

  if (perspective) {
    this->material.shader->SetUniform4fv("projMatrix", false, glm::value_ptr(renderer->projMatrix));
    this->material.shader->SetUniform4fv("cameraMatrix", false, glm::value_ptr(renderer->cameraMatrix));
  } else {
    glm::mat4 identity = glm::identity<glm::mat4>();
    this->material.shader->SetUniform4fv("projMatrix", false, glm::value_ptr(identity));
    this->material.shader->SetUniform4fv("cameraMatrix", false, glm::value_ptr(identity));
  }
  this->material.shader->SetUniform4f("color", this->material.color);
  if (this->material.hasTexture) {
    this->material.shader->SetTexture(renderer->maxBindableTextures, this->material.textureAddr, &renderer->textureMap);
  }
  GLCall(glDrawElements(GL_TRIANGLES, triangleCount*3, GL_UNSIGNED_INT, 0));
}

void SimpleRenderObject::UpdateTick(double deltaTime) {
  return;
}

bool SimpleRenderObject::CheckCollision(
  Renderer* renderer, 
  double x, 
  double y, 
  double* collisionZ, 
  CollisionInfo* info
) const {
  std::cout << "Checking collision inside simple renderer object" << std::endl;
  return false;
}

ClickEvent SimpleRenderObject::ProcessClick(CollisionInfo info) {
  std::cout << "Process click in simple render object" << std::endl;
  return {};
}
ClickEvent SimpleRenderObject::ProcessPreClick(CollisionInfo info) {
  std::cout << "Process pre-click in simple render object" << std::endl;
  return {};
}
void SimpleRenderObject::ReleaseClick() {
  std::cout << "Process release click in simple render object" << std::endl;
  return;
}
