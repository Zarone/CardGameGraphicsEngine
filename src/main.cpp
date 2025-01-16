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

int main(void)
{
  WindowManager myWindow = WindowManager();

  myWindow.SetupOpenGL();

  Renderer myRenderer(&myWindow);

  glm::mat4 projMatrix = glm::mat4(1.0f);
  glm::mat4 cameraMatrix = glm::mat4(1.0f);
  myRenderer.Setup3DTransforms(projMatrix, cameraMatrix);

  // CardGroup hand = CardGroup();
  // hand.AddCard("card3");
  // (
    VertexArray cardVao = VertexArray();
    cardVao.Bind();
    VertexBuffer cardBuffer = VertexBuffer(CardRenderingData::cardPositions, 5*4*sizeof(float));
    cardBuffer.Bind();
    MemoryLayout cardMemoryLayout = MemoryLayout();
    cardMemoryLayout.AddMemoryElement(GL_FLOAT, 3);
    cardMemoryLayout.AddMemoryElement(GL_FLOAT, 2);
    cardVao.AddBuffer(cardBuffer, cardMemoryLayout);
    IndexBuffer cardIndexBuffer = IndexBuffer(CardRenderingData::cardIndices, 2);
    cardIndexBuffer.Bind();
    Shader cardShader = Shader(myShaders::cardVertex, myShaders::cardFragment);
    cardShader.Bind();
    

    Texture cardTex = Texture("../assets/card3.png");
    //cardTex.Bind(0);
    //GLint cardTextureUniformID = cardShader.GetUniformLocation("frontCardTexture");
    //glUniform1i(cardTextureUniformID, 0);

    Texture backCardTex = Texture("../assets/back.png");
    //cardTex.Bind(1);
    //GLint backCardTextureUniformID = cardShader.GetUniformLocation("backCardTexture");
    //glUniform1i(backCardTextureUniformID, 1);
  // )

  cardShader.SetUniform4fv("projMatrix", false, glm::value_ptr(projMatrix));
  cardShader.SetUniform4fv("cameraMatrix", false, glm::value_ptr(cameraMatrix));
  
  double cursorX;
  double cursorY;

  glm::mat4 modelMatrix;
  glm::mat4 identity = glm::mat4(1.0f);

  /* Loop until the user closes the window */
  while (!myWindow.ShouldClose())
  {
    myWindow.GetCursorPosition(&cursorX, &cursorY);

    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cardVao.Bind();
    cardBuffer.Bind();
    cardIndexBuffer.Bind();
    cardShader.Bind();
    cardTex.Bind(3);
    //backCardTex.Bind(1);
    cardShader.SetUniform1i("cardTexture", 3);
    modelMatrix = glm::translate(identity, glm::vec3(0.0f, 0.5f, 1.0f));
    cardShader.SetUniform4fv("modelMatrix", false, glm::value_ptr(modelMatrix));
    GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

    /* Swap front and back buffers */
    myWindow.SwapBuffers();

    /* Poll for and process events */
    myWindow.PollEvents();
  }

  return 0;
}

