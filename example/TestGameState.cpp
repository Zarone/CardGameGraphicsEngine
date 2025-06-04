#include "../src/Helper/ErrorHandling.h"
#include "./TestGameState.h"
#include "./TestSceneData.h"
#include "./TestCardInfo.h"
#include "TestGameplayPiles.h"
#include "../src/shaders/allShaders.h"

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
      .shader=renderer->GetShader(ButtonShader),
      .color=glm::vec4(1.0f, 0, 0, 1.0f),
    }),
    std::bind(&TestGameState::EndTurnButtonPress, this)
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
  palette(renderer),
  database(database)
{

  oppHand.AddCard(0);
  oppHand.AddCard(2);
  oppHand.AddCard(3);
  oppHand.AddCard(0);
  oppHand.AddCard(2);
  oppHand.AddCard(3);

  hand.AddCard(0, 0);
  hand.AddCard(2, 1);
  hand.AddCard(3, 2);
  hand.AddCard(0, 3);
  hand.AddCard(2, 4);
  hand.AddCard(3, 5);
  hand.AddCard(0, 6);
  hand.AddCard(2, 7);
  hand.AddCard(3, 8);
  hand.AddCard(0, 9);
  hand.AddCard(1, 10);
  hand.AddCard(4, 11);
  hand.AddCard(5, 12);

  AddCardGroup(&hand, HAND);
  AddCardGroup(&oppHand, OPP_HAND);
  AddCardGroup(&reserve, RESERVE);
  AddCardGroup(&oppReserve, OPP_RESERVE);
  AddCardGroup(&battlefield, BATTLEFIELD);
  AddCardGroup(&oppBatlefield, OPP_BATTLEFIELD);
  AddCardGroup(&specials, SPECIALS);
  AddCardGroup(&oppSpecials, OPP_SPECIALS);

  renderer->textureMap.SetupTexturePath("endturn");
  AddObject(&palette);
  AddObject(&passTurn);
  AddCardGroup(&deck, DECK);
  AddCardGroup(&discardPile, DISCARD);

  this->LoadCommandPalette();
}

void TestGameState::LoadCommandPalette() {
  GameMode mode = this->gameplayManager.GetPhase();
  switch (mode) {
    case MY_TURN:
      palette.SetButtons({
        {
          .text="End Turn", 
          .func=std::bind(&TestGameState::EndTurnButtonPress, this)
        },
        {
          .text="Attack",
          .func=[]() {
            std::cout << "Attack declared" << std::endl;
          }
        }
      });
      break; 
    case OPPONENT_TURN:
      break; 
    case SELECTING_CARDS:
      palette.SetButtons({
        {
          .text="End Selection",
          .func=[]() {
            std::cout << "End Selection Pressed1" << std::endl;
          }
        },
        {
          .text="End Selection",
          .func=[]() {
            std::cout << "End Selection Pressed2" << std::endl;
          }
        },
        {
          .text="End Selection",
          .func=[]() {
            std::cout << "End Selection Pressed3" << std::endl;
          }
        },
        {
          .text="End Selection",
          .func=[]() {
            std::cout << "End Selection Pressed4" << std::endl;
          }
        },
        {
          .text="End Selection",
          .func=[]() {
            std::cout << "End Selection Pressed5" << std::endl;
          }
        },
      });
      break; 
    default:
      std::cout << "Unknown gamemode encountered while loading command palette" << std::endl;
      break;
  }
}

ClickEvent TestGameState::ProcessClick(CollisionInfo info) {
  SceneObject* src = (SceneObject*) info.groupPointer;

  if (src == &this->hand) {
    std::cout << "hand collision recognized" << std::endl;

    if (info.innerCollision == nullptr) {
      std::cout << "hand collision had no inner collision. Panic behavior." << std::endl;
      return {};
    }

    if (!info.innerCollision->isCard) {
      std::cout << "hand collision isn't with card, so not sure what to do. Panic behavior." << std::endl;
      return {};
    }

    unsigned int cardIndex = info.innerCollision->cardIndex;
    Card card = this->hand.GetCard(cardIndex);
    TestCardInfo* cardInfo = this->database->GetInfo(card.GetID());

    UpdateInfo update = this->gameplayManager.RequestUpdate({
      .type = SELECT_CARD,
      .selectedCard = card.GetGameID(),
      .from = HAND
    });

    if (update.phaseChange) {
      this->LoadCommandPalette();
    }

    for (CardMovement& move : update.movements) {
      CardGroup* from = cardGroupMap.at(move.from);
      CardGroup* to = cardGroupMap.at(move.to);
      from->MoveToGroupByGameID(move.cardId, to);
    }

    this->hand.ProcessClick(std::move(info));

  } else if (src == &this->oppHand) {
    std::cout << "opp hand collision recognized" << std::endl;
  } else if (src == &this->reserve) {
    std::cout << "reserve click recognized" << std::endl;
  } else if (src == &this->discardPile) {
    std::cout << "discard pile click recognized" << std::endl;

    if (info.isCard) {
      unsigned int cardIndex = info.cardIndex;
      std::cout << "Discard card with index " << cardIndex << " selected" << std::endl;
      Card card = this->discardPile.GetCard(cardIndex);
      std::cout << "Discard card with id " << card.GetGameID() << " selected" << std::endl;
      TestCardInfo* cardInfo = this->database->GetInfo(card.GetID());

      UpdateInfo update = this->gameplayManager.RequestUpdate({
        .type = SELECT_CARD,
        .selectedCard = card.GetGameID(),
        .from = DISCARD
      });

      for (CardMovement& move : update.movements) {
        CardGroup* from = cardGroupMap.at(move.from);
        CardGroup* to = cardGroupMap.at(move.to);
        from->MoveToGroupByGameID(move.cardId, to);
      }
    } else {
      std::cout << "discard collision had no card info. Sending to discard." << std::endl;
      src->ProcessClick(std::move(info));
    }
  } else if (src == &this->deck) {
    std::cout << "deck click recognized" << std::endl;
    src->ProcessClick(std::move(info));
  } else if (src == &this->palette) {
    this->palette.ProcessClick(std::move(info));
  } else {
    std::cout << "other click registered" << std::endl;
    src->ProcessClick(std::move(info));
  }

  return {
    .sceneSwap = false,
  };
}

void TestGameState::ProcessScroll(CollisionInfo info, double yOffset) {
  SceneObject* src = (SceneObject*) info.groupPointer;

  src->ProcessScroll(std::move(info), yOffset);
}

void TestGameState::EndTurnButtonPress() {
  std::cout << "end turn click" << std::endl;
}

void TestGameState::LoadProperShader(Renderer* renderer, CardGroup* group) {
  if (&this->hand == group) {
    for (CardItem& card : *group->GetCards()) {
      if (this->gameplayManager.IsSelectedCard(card.card.GetGameID())) {
        card.renderData.shader = renderer->GetShader(SelectedCardShader); 
      } else if (this->gameplayManager.IsPlayableCard(card.card.GetGameID())) {
        card.renderData.shader = renderer->GetShader(HighlightCardShader);
      } else {
        card.renderData.shader = renderer->GetShader(CardShader);
      }
    }
  } else if (&this->discardPile == group) {
    if (this->discardPile.GetIsExpanded()) {
      for (CardItem& card : *group->GetCards()) {
        if (this->gameplayManager.IsPlayableCard(card.card.GetGameID())) {
          card.renderData.shader = renderer->GetShader(HighlightCardShader);
        } else {
          card.renderData.shader = renderer->GetShader(CardShader);
        }
      }
    } else {
      std::vector<CardItem>* cards = group->GetCards();
      auto startIter = cards->begin();
      auto endIter = cards->end();
      int groupSize = cards->size();
      if (groupSize != 0) {
        for (auto iter = startIter; iter < endIter-1; ++iter) {
          (*iter).renderData.shader = renderer->GetShader(CardShader);
        }

        cards->at(groupSize-1).renderData.shader = renderer->GetShader(HighlightCardShader);
      }
    }
  } else {
    for (auto& card : *group->GetCards()) {
      card.renderData.shader = renderer->GetShader(CardShader);
    }
  }
}

void TestGameState::Render(Renderer* renderer) {
  for (CardGroup* group : this->cardGroups) {
    this->LoadProperShader(renderer, group);
  }
  GameState::Render(renderer);
}
