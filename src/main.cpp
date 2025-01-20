#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../include/WindowManager.h"
#include "../include/Renderer.h"
#include "../include/CardGroup.h"
#include "../include/TextureMap.h"
#include "../include/Scene.h"

int main(void)
{
  WindowManager myWindow = WindowManager();

  myWindow.SetupOpenGL();

  GLint maxBindableTextures;
  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxBindableTextures);
  Renderer myRenderer(&myWindow, maxBindableTextures);

  myRenderer.Setup3DTransforms(
    glm::vec3(0.0f, 0.0f, 10.f),
    glm::vec3(0.0f, 0.0f, -1.0f)
  );

  Scene scene(&myRenderer);

  TextureMap textureMap = TextureMap();
  CardGroup hand(
    &textureMap,
    glm::vec3(0.0f, -2.0f, 4.0f),
    //glm::vec3(0.0f, -0.5f, 6.0f),
    -20.0f, // rotateX
    4.0f, // width
    false // z flipped
  );

  CardGroup oppHand(
    &textureMap,
    glm::vec3(0.0f, 2.0f, 1.0f),
    40.0f, // rotateX
    4.0f, // width
    true  // z flipped
  );

  scene.AddObject(&hand);
  scene.AddObject(&oppHand);

  oppHand.AddCard(0);
  oppHand.AddCard(2);
  oppHand.AddCard(3);

  hand.AddCard(0);
  hand.AddCard(1);
  hand.AddCard(2);
  hand.AddCard(3);
  hand.AddCard(0);
  hand.AddCard(1);
  hand.AddCard(2);
  //hand.AddCard(3);
  //hand.AddCard(0);
  //hand.AddCard(1);
  //hand.AddCard(2);
  //hand.AddCard(3);

  glm::mat4 modelMatrix;
  glm::mat4 identity = glm::mat4(1.0f);

  RenderData handRenderingData = {
    .isHand = true,
  };

  auto currentTime = std::chrono::high_resolution_clock::now();
  auto lastTime = currentTime;
  auto diffTime = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(currentTime - lastTime);
  double deltaTime = 0;

  scene.SetupMouseClickCallback(&myWindow);

  /* Loop until the user closes the window */
  while (!myWindow.ShouldClose())
  {
    
    currentTime = std::chrono::high_resolution_clock::now();
    diffTime = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(currentTime - lastTime);
    deltaTime = diffTime.count()/1000.0f;
    lastTime = currentTime;

    myWindow.GetCursorPosition(&handRenderingData.cursorX, &handRenderingData.cursorY);

    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    hand.Render(&myRenderer, handRenderingData);
    hand.UpdateTick(deltaTime);

    oppHand.Render(&myRenderer, handRenderingData);
    oppHand.UpdateTick(deltaTime);

    /* Swap front and back buffers */
    myWindow.SwapBuffers();

    /* Poll for and process events */
    myWindow.PollEvents();
  }

  return 0;
}

