#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "../include/shaders/allShaders.h"
#include "../include/WindowManager.h"
#include "../include/Shader.h"
#include "../include/ErrorHandling.h"
#include "../include/VertexBuffer.h"
#include "../include/IndexBuffer.h"
#include "../include/VertexArray.h"
#include "../include/Texture.h"
#include "../include/Renderer.h"
#include "../include/CardRenderingData.h"
#include "../include/CardGroup.h"
#include "../include/TextureMap.h"

int main(void)
{
  WindowManager myWindow = WindowManager();

  myWindow.SetupOpenGL();

  Renderer myRenderer(&myWindow);

  myRenderer.Setup3DTransforms(
    glm::vec3(0.0f, 0.0f, 5.5f),
    glm::vec3(0.0f, 0.0f, -1.0f)
  );

  TextureMap textureMap = TextureMap();
  CardGroup hand(
    &textureMap,
    glm::vec3(0.0f, 0.0f, 1.5f),
    0.0f, // rotateX
    0.0f, // rotateZ
    2.f, // width
    false // z flipped
  );
  hand.AddCard(0);
  hand.AddCard(1);
  hand.AddCard(2);
  hand.AddCard(3);

  hand.PrepareTextures();
  
  double cursorX;
  double cursorY;

  glm::mat4 modelMatrix;
  glm::mat4 identity = glm::mat4(1.0f);

  GLint maxBindableTextures;
  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxBindableTextures);
  std::cout << "maxBindableTextures: " << maxBindableTextures << std::endl;

  /* Loop until the user closes the window */
  while (!myWindow.ShouldClose())
  {
    myWindow.GetCursorPosition(&cursorX, &cursorY);

    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    hand.Render(maxBindableTextures, myRenderer);

    /* Swap front and back buffers */
    myWindow.SwapBuffers();

    /* Poll for and process events */
    myWindow.PollEvents();
  }

  return 0;
}

