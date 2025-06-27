#include "../src/Helper/ErrorHandling.h"
#include "./TestGameState.h"
#include "./TestSceneData.h"
#include "./TestCardInfo.h"
#include "TestGameplayPiles.h"
#include "../src/shaders/allShaders.h"
#include <thread>
#include "../src/SimpleRenderObjects/TextBox.h"

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
  battlefield(
    renderer,
    glm::vec3(0.0f, -2.0f, 1.f),
    //glm::vec3(0.0f, -0.5f, 6.0f),
    -90.0f, // rotateX
    7.0f,   // width
    false,  // z flipped
    false
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
  oppHand(
    renderer,
    glm::vec3(0.0f, -1.5f, -5.5f),
    10.0f, // rotateX
    4.0f,  // width
    true, // z flipped
    true
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
  oppBatlefield(
    renderer,
    glm::vec3(0.0f, -2.0f, -1.f),
    //glm::vec3(0.0f, -0.5f, 6.0f),
    -90.0f, // rotateX
    7.0f,   // width
    false,  // z flipped
    false
  ),
  oppDeck(
    renderer,
    glm::vec3(-4.1f, -2.0f, -2.75f),
    -90.0f,
    true
  ),
  oppDiscardPile(
    renderer,
    glm::vec3(4.1f, -2.0f, -4.5f),
    -90.0f,
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
  tempPile(
    renderer,
    glm::vec3(0,0,0)
  ),
  palette(renderer),
  database(database)
{
  AddCardGroup(&hand, "HAND");
  AddCardGroup(&oppHand, "OPP_HAND");
  AddCardGroup(&reserve, "RESERVE");
  AddCardGroup(&oppReserve, "OPP_RESERVE");
  AddCardGroup(&battlefield, "BATTLEFIELD");
  AddCardGroup(&oppBatlefield, "OPP_BATTLEFIELD");
  AddCardGroup(&specials, "SPECIALS");
  AddCardGroup(&oppSpecials, "OPP_SPECIALS");

  renderer->textureMap.SetupTexturePath("endturn");
  AddObject(&palette);
  AddObject(&passTurn);
  AddCardGroup(&this->deck, "DECK");
  AddCardGroup(&discardPile, "DISCARD");
  AddCardGroup(&tempPile, "TEMPORARY");
  AddCardGroup(&oppDeck, "OPP_DECK");
  AddCardGroup(&oppDiscardPile, "OPP_DISCARD");

  this->LoadCommandPalette();

  std::vector<unsigned int> deck = {
    0, 1, 2, 3, 4, 5,
    0, 1, 2, 3, 4, 5,
    0, 1, 2, 3, 4, 5,
    0, 1, 2, 3, 4, 5,
    0, 1, 2, 3, 4, 5,
    0, 1, 2, 3, 4, 5,
    0, 1, 2, 3, 4, 5,
    0, 1, 2, 3, 4, 5,
    0, 1, 2, 3, 4, 5,
    0, 1, 2, 3, 4, 5,
  };

  this->setupThread = std::thread(
    [this, deck](){
      this->Setup(deck);
    }
  );

  // Create setupTextBox for waiting state
  glm::mat4 boxTransform = glm::scale(
    glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0f, 0.0f, 0.0f)),
    glm::vec3(1.9f, 1.9f, 1.0f)
  );
  Material boxMaterial = Material({
    .hasTexture = false,
    .shader = renderer->GetShader(BasicShader),
    .color = glm::vec4(0.0f, 0.0f, 1.0f, 0.7f)
  });
  setupTextBox = new TextBox(
    renderer,
    boxTransform,
    boxMaterial,
    "../assets/fonts/Roboto-Regular.ttf",
    32,
    "Waiting for player to join...",
    glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
  );
}

void TestGameState::Setup(std::vector<unsigned int> deck) {
  SetupData setupData = this->gameplayManager.Setup(deck, &this->stopSetupThread);
  if (this->stopSetupThread) return;
  for (auto gameID : setupData.correspondingGameIds) {
    this->deck.AddCard(this->gameplayManager.gameIDToID[gameID], gameID);
  }
  for (auto gameID : setupData.oppCorrespondingGameIds) {
    this->oppDeck.AddCard(gameID);
  }
  this->HandleUpdate(setupData.info);
  this->setupComplete = true;
}

TestGameState::~TestGameState() {
  std::cout << "delete game state" << std::endl;
  stopSetupThread = true;
  if (setupThread.joinable()) {
    setupThread.join();
  } 
  
  if (setupTextBox) delete setupTextBox;
}
void TestGameState::Render(Renderer* renderer) {
  if (!this->setupComplete) {
    if (setupTextBox) setupTextBox->Render(renderer);
    else std::cout << "Waiting for player to join" << std::endl;
    return;
  }

  this->ProcessPendingMessages();

  for (CardGroup* group : this->cardGroups) {
    this->LoadProperShader(renderer, group);
  }
  GameState::Render(renderer);
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
      palette.SetButtons({
        {
          .text="Say Hi",
          .func=[]() {
            std::cout << "Clicked \"Say Hi\" button" << std::endl;
          }
        }
      });
      break; 
    case SELECTING_CARDS:
      if (this->gameplayManager.SelectionPossiblyDone()) {
        palette.SetButtons({
          {
            .text="End Selection",
            .func=[this]() {
              this->ProcessAction({
                .type = FINISH_SELECTION,
              });
            }
          },
        });
      } else {
        palette.SetButtons({
        });
      }
      break; 
    default:
      std::cout << "Unknown gamemode encountered while loading command palette" << std::endl;
      break;
  }
}

ClickEvent TestGameState::ProcessClick(CollisionInfo info) {
  if (!this->setupComplete) return {};

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

    this->ProcessAction({
      .type = SELECT_CARD,
      .selectedCards = { card.GetGameID() },
      .from = Pile("HAND")
    });

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

      this->discardPile.SetDirtyPosition(true);
      this->ProcessAction({
        .type = SELECT_CARD,
        .selectedCards = { card.GetGameID() },
        .from = Pile("DISCARD")
      });
    } else {
      std::cout << "discard collision had no card info. Sending to discard." << std::endl;
      src->ProcessClick(std::move(info));
    }
  } else if (src == &this->deck) {
    std::cout << "deck click recognized" << std::endl;

    if (info.isCard) {
      unsigned int cardIndex = info.cardIndex;
      std::cout << "card with index " << cardIndex << " selected" << std::endl;
      Card card = this->deck.GetCard(cardIndex);
      std::cout << "card with id " << card.GetGameID() << " selected" << std::endl;
      TestCardInfo* cardInfo = this->database->GetInfo(card.GetID());

      this->deck.SetDirtyPosition(true);
      this->ProcessAction({
        .type = SELECT_CARD,
        .selectedCards = { card.GetGameID() },
        .from = Pile("DECK")
      });
    } else {
      std::cout << "deck collision had no card info. Sending to deck." << std::endl;
      src->ProcessClick(std::move(info));
    }
  } else if (src == &this->oppDeck) {
    std::cout << "Clicked Opponent's Deck" << std::endl;
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

void TestGameState::UpdateTick(double deltaTime) {
  if (!this->setupComplete) return;
  this->GameState::UpdateTick(deltaTime);
}

void TestGameState::ProcessScroll(CollisionInfo info, double yOffset) {
  if (!this->setupComplete) return;
  SceneObject* src = (SceneObject*) info.groupPointer;

  src->ProcessScroll(std::move(info), yOffset);
}

void TestGameState::EndTurnButtonPress() {
  std::cout << "end turn click" << std::endl;
}

void TestGameState::HandleUpdate(const UpdateInfo& update) {
  bool phaseUpdate = update.selectedCardsChanged || this->gameplayManager.GetPhase() != update.phase;
  this->gameplayManager.ChangePhaseForUpdate(update);
  if (phaseUpdate) {
     this->LoadCommandPalette();
  }

  if (update.openView == "TEMPORARY") {
    this->tempPile.EnableWithCards(update.openViewCards);
  }

  for (const CardMovement& move : update.movements) {
    std::cout << "Move: " << move.gameID << std::endl;
    CardGroup* from = cardGroupMap.at(move.from.GetInString());
    CardGroup* to = cardGroupMap.at(move.to.GetInString());
    from->MoveToGroupByGameID(move.gameID, to, move.cardID, move.from == "OPP_HAND");
  }
}


void TestGameState::ProcessPendingMessages() {
  // Check for and process any pending messages from the server
  if (this->gameplayManager.HasPendingMessages()) {
    std::vector<UpdateInfo> messages = this->gameplayManager.GetAllMessages();
    std::cout << "Processing " << messages.size() << " pending messages" << std::endl;
    for (const UpdateInfo& info : messages) {
      this->HandleUpdate(info);
    }
  }
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
  } else if (&this->discardPile == group || &this->deck == group) {
    ExpandableStackCardGroup* pileGroup = (ExpandableStackCardGroup*)group;
    if (pileGroup->GetIsExpanded()) {
      for (CardItem& card : *group->GetCards()) {
        if (this->gameplayManager.IsSelectedCard(card.card.GetGameID())) {
          card.renderData.shader = renderer->GetShader(SelectedCardShader); 
        } else if (this->gameplayManager.IsPlayableCard(card.card.GetGameID())) {
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

void TestGameState::ProcessAction(const GameAction& action) {
  this->gameplayManager.PostAction(action);
}
