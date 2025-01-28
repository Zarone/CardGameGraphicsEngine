cmake_minimum_required(VERSION 3.31)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE "DEBUG")
set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
project(GraphicsEngine LANGUAGES CXX)
include(FetchContent)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm
)
FetchContent_MakeAvailable(glm)

find_package(GLEW)

# Add the executable with all source files
add_executable(${PROJECT_NAME}
  example/TestCardDatabaseSingleton.cpp
  example/TestCardInfo.cpp
  example/TestGameState.cpp
  example/TestScene.cpp
  src/external/stb_image.cpp
  src/shaders/BasicVertex.cpp
  src/shaders/BasicFragment.cpp
  src/shaders/CardVertex.cpp
  src/shaders/CardFragment.cpp
  src/shaders/HighlightedCardVertex.cpp
  src/shaders/HighlightedCardFragment.cpp
  src/shaders/TextureVertex.cpp
  src/shaders/TextureFragment.cpp
  src/CardGroup/CardGroup.cpp
  src/CardGroup/CardGroupRenderer.cpp
  src/CardGroup/CardRenderingData.cpp
  src/CardGroup/ExpandableStackCardGroup.cpp
  src/CardGroup/FannedCardGroup.cpp
  src/CardGroup/FannedCardGroupRenderer.cpp
  src/CardGroup/StackedCardGroupRenderer.cpp
  src/HelperOpenGL/IndexBuffer.cpp
  src/HelperOpenGL/MemoryLayout.cpp
  src/HelperOpenGL/VertexArray.cpp
  src/HelperOpenGL/VertexBuffer.cpp
  src/SimpleRenderObjects/BoundButton.cpp
  src/SimpleRenderObjects/SimpleButton.cpp
  src/SimpleRenderObjects/SimplePlane.cpp
  src/SimpleRenderObjects/SimpleRenderObject.cpp
  src/Card.cpp
  src/CardInfo.cpp
  src/ErrorHandling.cpp
  src/GameState.cpp
  src/MathHelper.cpp
  src/Renderer.cpp
  src/Scene.cpp
  src/Shader.cpp
  src/Texture.cpp
  src/TextureLRU.cpp
  src/TextureMap.cpp
  src/WindowManager.cpp
  src/main.cpp
)

add_executable(test
  src/external/stb_image.cpp
  test/main.cpp
  test/TestTextureLRU.cpp
  src/TextureLRU.cpp
  src/Texture.cpp
  src/ErrorHandling.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw GLEW::GLEW glm)
target_link_libraries(test PRIVATE GLEW::GLEW)

if (APPLE)
  target_link_libraries(${PROJECT_NAME} PRIVATE "-framework OpenGL")
endif()

