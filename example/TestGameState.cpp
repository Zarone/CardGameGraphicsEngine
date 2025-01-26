#include "../include/ErrorHandling.h"
#include "./TestGameState.h"
#include "./TestSceneData.h"
#include "./TestCardInfo.h"

TestGameState::TestGameState(Renderer* renderer, TestCardDatabaseSingleton* database):
  hand(
    renderer,
    glm::vec3(0.0f, -1.5f, 6.25f),
    //glm::vec3(0.0f, -0.5f, 6.0f),
    -30.0f, // rotateX
    4.0f,   // width
    false,  // z flipped
    true
  ),
  oppHand(
    renderer,
    glm::vec3(0.0f, -1.5f, -5.5f),
    10.0f, // rotateX
    4.0f,  // width
    true, // z flipped
    true
  ),
  specials(
    renderer,
    glm::vec3(0.0f, -2.0f, 4.5f),
    //glm::vec3(0.0f, -0.5f, 6.0f),
    -90.0f, // rotateX
    7.0f,   // width
    false,  // z flipped
    false
  ),
  oppSpecials(
    renderer,
    glm::vec3(0.0f, -2.0f, -4.5f),
    //glm::vec3(0.0f, -0.5f, 6.0f),
    -90.0f, // rotateX
    7.0f,   // width
    false,  // z flipped
    false
  ),
  reserve(
    renderer,
    glm::vec3(0.0f, -2.0f, 2.75f),
    //glm::vec3(0.0f, -0.5f, 6.0f),
    -90.0f, // rotateX
    7.0f,   // width
    false,  // z flipped
    false
  ),
  oppReserve(
    renderer,
    glm::vec3(0.0f, -2.0f, -2.75f),
    //glm::vec3(0.0f, -0.5f, 6.0f),
    -90.0f, // rotateX
    7.0f,   // width
    false,  // z flipped
    false
  ),
  battlefield(
    renderer,
    glm::vec3(0.0f, -2.0f, 1.f),
    //glm::vec3(0.0f, -0.5f, 6.0f),
    -90.0f, // rotateX
    7.0f,   // width
    false,  // z flipped
    false
  ),
  oppBatlefield(
    renderer,
    glm::vec3(0.0f, -2.0f, -1.f),
    //glm::vec3(0.0f, -0.5f, 6.0f),
    -90.0f, // rotateX
    7.0f,   // width
    false,  // z flipped
    false
  ),
  passTurn(
    renderer,
    glm::rotate(
      glm::translate(
        glm::identity<glm::mat4>(), 
        glm::vec3(4.1f, -1.999f, 1.f)
      ),
      glm::radians(-90.0f), 
      glm::vec3(1.0f, 0, 0)
    ),
    Material({
      .hasTexture=true,
      .textureMap=&renderer->textureMap,
      .textureAddr="endturn",
      .shader=renderer->GetShader("buttonShader"),
      .color=glm::vec4(1.0f, 0, 0, 1.0f),
    }),
    std::bind(&TestGameState::test, this)
  ),
  deck(
    renderer,
    glm::vec3(4.1f, -2.0f, 2.75f),
    -90.0f,
    true
  ),
  discardPile(
    renderer,
    glm::vec3(4.1f, -2.0f, 4.5f),
    -90.0f,
    false
  ),
  database(database)
{

  oppHand.AddCard(0);
  oppHand.AddCard(2);
  oppHand.AddCard(3);
  oppHand.AddCard(0);
  oppHand.AddCard(2);
  oppHand.AddCard(3);

  hand.AddCard(0);
  hand.AddCard(2);
  hand.AddCard(3);
  hand.AddCard(0);
  hand.AddCard(2);
  hand.AddCard(3);
  hand.AddCard(0);
  hand.AddCard(2);
  hand.AddCard(3);

  AddObject(&hand);
  AddObject(&oppHand);
  AddObject(&reserve);
  AddObject(&oppReserve);
  AddObject(&battlefield);
  AddObject(&oppBatlefield);
  AddObject(&specials);
  AddObject(&oppSpecials);

  renderer->textureMap.SetupTexturePath("endturn");
  AddObject(&passTurn);
  AddObject(&deck);
  AddObject(&discardPile);
}

ClickEvent TestGameState::ProcessClick(CollisionInfo info) {
  SceneObject* src = (SceneObject*) info.groupPointer;

  if (src == &this->hand) {
    std::cout << "hand collision recognized" << std::endl;

    unsigned int cardIndex = info.collisionIndex;
    Card card = this->hand.GetCard(cardIndex);
    TestCardInfo* cardInfo = this->database->GetInfo(card.GetID());
    if (cardInfo->type == TestCardInfo::CardType::BASIC_CHARACTER_CARD) {
      std::cout << "playing basic character" << std::endl;
      this->hand.MoveToGroup(info.collisionIndex, &this->discardPile);
    } else if (cardInfo->type == TestCardInfo::CardType::SPECIAL_CHARACTER_CARD) {
      std::cout << "how do you have that in your hand... :(" << std::endl;
      this->hand.MoveToGroup(info.collisionIndex, &this->deck);
    }

  } else if (src == &this->oppHand) {
    std::cout << "opp hand collision recognized" << std::endl;
  } else if (src == &this->reserve) {
    std::cout << "reserve click recognized" << std::endl;
  } else {
    src->ProcessClick(info);
  }

  return {
    .sceneSwap = false,
  };
}

void TestGameState::test() {
  std::cout << "end turn click" << std::endl;
}

void TestGameState::LoadProperShader(Renderer* renderer, CardGroup* group) {
  int highlight = 0;
  if (&this->hand == group) {
    for (CardItem& card : *group->GetCards()) {
      if (database->GetInfo(card.card.GetID())->type == TestCardInfo::CardType::BASIC_CHARACTER_CARD) {
        card.renderData.shader = renderer->GetShader("highlightCardShader");
        highlight++;
      } else {
        card.renderData.shader = renderer->GetShader("cardShader");
      }
    }
  } else if (&this->discardPile == group) {
    std::vector<CardItem>* cards = group->GetCards();
    auto startIter = cards->begin();
    auto endIter = cards->end();
    int groupSize = cards->size();
    if (groupSize != 0) {
      for (auto iter = startIter; iter < endIter-1; ++iter) {
        (*iter).renderData.shader = renderer->GetShader("cardShader");
      }
      cards->at(groupSize-1).renderData.shader = renderer->GetShader("highlightCardShader");
      highlight++;
    }
  } else {
    for (auto& card : *group->GetCards()) {
      card.renderData.shader = renderer->GetShader("cardShader");
    }
  }
  group->highlightedCards=highlight;
}

void TestGameState::Render(Renderer* renderer) {
  for (CardGroup* group : this->cardGroups) {
    this->LoadProperShader(renderer, group);
  }
  GameState::Render(renderer);
}
