#include <GL/glew.h>

#include "../include/WindowManager.h"

#include "../example/TestCardDatabaseSingleton.h"
#include "../example/TestGameState.h"
#include "../example/TestScene.h"

int main(void)
{
  WindowManager myWindow;
  myWindow.SetupOpenGL();

  TestCardDatabaseSingleton database;

  TestScene scene(&myWindow);
  scene.SetupCamera(
    glm::vec3(0.0f, 0.0f, 10.f),
    glm::vec3(0.0f, 0.0f, -1.0f)
  );
  scene.SetupCardDataBase(&database);
  scene.AddObject<TestGameState>();

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

    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    scene.Render();
    scene.UpdateTick(deltaTime);

    /* Swap front and back buffers */
    myWindow.SwapBuffers();

    /* Poll for and process events */
    myWindow.PollEvents();
  }

  return 0;
}

