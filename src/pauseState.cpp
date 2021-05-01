#define LOG_TAG "PauseState"

#include "pauseState.h"
#include "button.h"
#include "buttonView.h"
#include "componentDefinition.h"
#include "core.h"
#include "label.h"
#include "resourceManager.h"
#include "stateManager.h"
#include "stateType.h"
#include "utils.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Event.hpp>

const sf::Color PauseState::mBackgroundColor = {0, 0, 0, 150};

const ComponentDefinition PauseState::mHeaderLabelDefinition{
    "Pause",
    ComponentDefinition::mDefaultFontName,
    STATE_HEADER_COLOR,
    STATE_CHAR_SIZE,
    {0.5f * WINDOW_WIDTH, 0.5f * WINDOW_HEIGHT}};

const ComponentDefinition PauseState::mContinueButtonDefinition{
    "Continue", {0.5f * WINDOW_WIDTH, 0.62f * WINDOW_HEIGHT}};
const ComponentDefinition PauseState::mGoToMenuButtonDefinition{
    "Go to menu", {0.5f * WINDOW_WIDTH, 0.74f * WINDOW_HEIGHT}};

PauseState::PauseState(StateManager &stateManager)
    : StateBase{stateManager},
      mHeaderLabel(makeUnique<Label>(mHeaderLabelDefinition)), mButtonView(),
      mBackgroundTexture(makeUnique<sf::Texture>()),
      mBackgroundSprite(makeUnique<sf::Sprite>()),
      mBackgroundRectangle(makeUnique<sf::RectangleShape>(
          sf::Vector2f{static_cast<float>(WINDOW_WIDTH),
                       static_cast<float>(WINDOW_HEIGHT)})) {
  CHECK(mBackgroundTexture->create(WINDOW_WIDTH, WINDOW_HEIGHT));

  mBackgroundTexture->update(ResourceManager::getWindow());
  mBackgroundSprite->setTexture(*mBackgroundTexture);
  mBackgroundRectangle->setFillColor(mBackgroundColor);

  ButtonView::ButtonList buttons;

  buttons.emplace_back(std::move(mContinueButtonDefinition),
                       [this] { resumeGame(); });
  buttons.emplace_back(std::move(mGoToMenuButtonDefinition), [this] {
    getStateManager().requestStateTranstion(STATE_TYPE::MENU);
  });

  mButtonView = makeUnique<ButtonView>(std::move(buttons));
}

PauseState::~PauseState() {}

void PauseState::handleEvent(const sf::Event &event) {
  if (event.type == sf::Event::KeyPressed &&
      event.key.code == sf::Keyboard::Escape) {
    resumeGame();
  } else {
    mButtonView->handleEvent(event);
  }
}

void PauseState::resumeGame() {
  getStateManager().requestStateTranstion(STATE_TYPE::GAME);
}

void PauseState::draw(sf::RenderTarget &target, sf::RenderStates states) const {
  target.draw(*mBackgroundSprite, states);
  target.draw(*mBackgroundRectangle, states);
  target.draw(*mHeaderLabel, states);
  target.draw(*mButtonView, states);
}
