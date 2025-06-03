#pragma once

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "WindowManager.h"
#include "TextureMap.h"
#include "../Card/CardDatabaseSingleton.h"
#include "../Helper/CursorData.h"
#include "Shader.h"
#include "../shaders/allShaders.h"

class Renderer
{
private:
  WindowManager* window;
public:
  std::unordered_map<ShaderID, Shader> shaderMap;
  TextureMap textureMap;
  glm::mat4 projMatrix;
  glm::mat4 cameraMatrix;
  int maxBindableTextures;

  Renderer(WindowManager* window);

  void Setup3DTransforms(const glm::vec3& cameraPosition, const glm::vec3& cameraDirection);

  void SetupDatabaseForTexturing(CardDatabaseSingleton* database);

  bool InsideWindowBounds(double posX, double posY) const;

  /*
  *
  * @brief Takes the vec4 position from projecting point
  * onto screen (x between -1 and 1), and gives the exact 
  * pixel coordinate on the screen.
  *
  */
  glm::vec2 GetScreenPositionFromCamera(glm::vec4 screenCoords);

  glm::vec3 Get3DScreenPositionFromCamera(glm::vec4 screenCoords);

  void GetCursorPosition(CursorData* cursorData);

  void SetupShader(const ShaderID id, const std::string& vertex, const std::string& fragment);

  void ResetShaders();

  Shader* GetShader(const ShaderID id);

  float GetAspectRatio();
};
