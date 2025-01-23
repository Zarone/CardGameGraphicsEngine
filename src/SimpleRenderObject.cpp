#include <glm/gtc/type_ptr.hpp>
#include "../include/SimpleRenderObject.h"
#include "../include/ErrorHandling.h"

SimpleRenderObject::SimpleRenderObject(
  const std::string& vertexShader,
  const std::string& fragmentShader,
  glm::mat4 transform,
  Material material
):
  shader(vertexShader, fragmentShader),
  color(color)
{
  this->SetTransform(&transform);
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
  this->shader.Bind();
  this->shader.SetUniform4fv("modelMatrix", false, glm::value_ptr(*transform));
}

void SimpleRenderObject::Render(Renderer* renderer) {
  this->vArray.Bind();
  this->iBuffer.Bind();
  this->shader.Bind();
  this->shader.SetUniform4fv("projMatrix", false, glm::value_ptr(renderer->projMatrix));
  this->shader.SetUniform4fv("cameraMatrix", false, glm::value_ptr(renderer->cameraMatrix));
  this->shader.SetUniform4f("color", this->color);
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
