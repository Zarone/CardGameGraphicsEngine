cmake_minimum_required(VERSION 3.31)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE "DEBUG")
set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
project(GraphicsEngine LANGUAGES CXX)

# Enable precompiled headers
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpch-instantiate-templates")

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
find_package(Freetype REQUIRED)

# Add the executable with all source files
add_executable(${PROJECT_NAME}
  src/pch.cpp
  example/ServerHandler/ServerManager.cpp
  example/external/easywsclient.cpp
  example/TestCardDatabaseSingleton.cpp
  example/TestCardInfo.cpp
  example/TestGameState.cpp
  example/TestGameplayManager.cpp
  example/TestGameplayPiles.cpp
  example/TestGameplayPhase.cpp
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
  src/shaders/SelectedCardFragment.cpp
  src/CardGroup/CardItem.cpp
  src/CardGroup/CardGroup.cpp
  src/CardGroup/CardGroupRenderer.cpp
  src/CardGroup/CardRenderingData.cpp
  src/CardGroup/ExpandableStackCardGroup.cpp
  src/CardGroup/FannedCardGroup.cpp
  src/CardGroup/FannedCardGroupRenderer.cpp
  src/CardGroup/StackedCardGroupRenderer.cpp
  src/CardGroup/ExpandedStackCardGroupRenderer.cpp
  src/CardGroup/TemporaryCardGroup.cpp
  src/HelperOpenGL/IndexBuffer.cpp
  src/HelperOpenGL/MemoryLayout.cpp
  src/HelperOpenGL/VertexArray.cpp
  src/HelperOpenGL/VertexBuffer.cpp
  src/SimpleRenderObjects/BoundButton.cpp
  src/SimpleRenderObjects/SimpleButton.cpp
  src/SimpleRenderObjects/SimplePlane.cpp
  src/SimpleRenderObjects/SimpleRenderObject.cpp
  src/SimpleRenderObjects/TextBox.cpp
  src/Card/Card.cpp
  src/Card/CardInfo.cpp
  src/Helper/ErrorHandling.cpp
  src/Scene/GameState.cpp
  src/Scene/CommandPalette.cpp
  src/Helper/MathHelper.cpp
  src/HelperRendering/Renderer.cpp
  src/Scene/Scene.cpp
  src/HelperRendering/Shader.cpp
  src/HelperRendering/Texture.cpp
  src/HelperRendering/TextureLRU.cpp
  src/HelperRendering/TextureMap.cpp
  src/HelperRendering/TextRenderer.cpp
  src/HelperRendering/WindowManager.cpp
  src/main.cpp
)

# Link libraries first
target_link_libraries(${PROJECT_NAME} PRIVATE glfw GLEW::GLEW glm Freetype::Freetype)

# Enable precompiled headers for the main executable (after linking libraries)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch.h)

add_executable(test
  src/pch.cpp
  src/external/stb_image.cpp
  test/main.cpp
  test/TestTextureLRU.cpp
  src/HelperRendering/TextureLRU.cpp
  src/HelperRendering/Texture.cpp
  src/Helper/ErrorHandling.cpp
)

# Link libraries for test executable
target_link_libraries(test PRIVATE GLEW::GLEW glm)

# Enable precompiled headers for the test executable (after linking libraries)
target_precompile_headers(test PRIVATE src/pch.h)

if (APPLE)
  target_link_libraries(${PROJECT_NAME} PRIVATE "-framework OpenGL")
endif()

