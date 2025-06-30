#include "pch.h"

#include "../src/HelperRendering/WindowManager.h"

#include "../example/TestCardDatabaseSingleton.h"
#include "../example/TestScene.h"
#include "../example/TestSceneData.h"

int main(void)
{
  WindowManager myWindow;
  myWindow.SetupOpenGL();

  TestCardDatabaseSingleton database;

  TestScene scene(&myWindow, &database);

  scene.Swap(TestSceneID::MAIN_MENU);

  scene.SetupMouseClickCallback(&myWindow);
  scene.SetupScrollCallback(&myWindow);

  /* Loop until the user closes the window */
  while (!myWindow.ShouldClose())
  {
    scene.ClearScreen();

    scene.Render();
    scene.UpdateTick(myWindow.DeltaTime());

    /* Swap front and back buffers */
    myWindow.SwapBuffers();

    /* Poll for and process events */
    myWindow.PollEvents();
  }

  return 0;
}

