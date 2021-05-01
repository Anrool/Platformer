#define LOG_TAG "MenuState"

#include "menuState.h"
#include "button.h"
#include "buttonView.h"
#include "componentDefinition.h"
#include "core.h"
#include "label.h"
#include "resourceManager.h"
#include "stateManager.h"
#include "stateType.h"
#include "utils.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

const ComponentDefinition MenuState::mHeaderLabelDefinition{
    "Menu",
    ComponentDefinition::mDefaultFontName,
    STATE_HEADER_COLOR,
    STATE_CHAR_SIZE,
    {0.5f * WINDOW_WIDTH, 0.3f * WINDOW_HEIGHT}};

const ComponentDefinition MenuState::mPlayButtonDefinition{
    "Play", {0.5f * WINDOW_WIDTH, 0.4f * WINDOW_HEIGHT}};
const ComponentDefinition MenuState::mSettingsButtonDefinition{
    "Settings", {0.5f * WINDOW_WIDTH, 0.52f * WINDOW_HEIGHT}};
const ComponentDefinition MenuState::mExitButtonDefinition{
    "Exit", {0.5f * WINDOW_WIDTH, 0.64f * WINDOW_HEIGHT}};

MenuState::MenuState(StateManager &stateManager)
    : StateBase{stateManager},
      mHeaderLabel(makeUnique<Label>(mHeaderLabelDefinition)), mButtonView(),
      mSprite(makeUnique<sf::Sprite>(
          ResourceManager::getTexture(STATE_TEXTURE_NAME))) {
  ButtonView::ButtonList buttons;

  buttons.emplace_back(std::move(mPlayButtonDefinition), [this] {
    getStateManager().requestStateTranstion(STATE_TYPE::GAME);
  });
  buttons.emplace_back(std::move(mSettingsButtonDefinition), [this] {
    getStateManager().requestStateTranstion(STATE_TYPE::SETTINGS);
  });
  buttons.emplace_back(std::move(mExitButtonDefinition), [this] {
    getStateManager().requestStateTranstion(STATE_TYPE::EXIT);
  });

  mButtonView = makeUnique<ButtonView>(std::move(buttons));
}

MenuState::~MenuState() {}

void MenuState::handleEvent(const sf::Event &event) {
  mButtonView->handleEvent(event);
}

void MenuState::draw(sf::RenderTarget &target, sf::RenderStates states) const {
  target.draw(*mSprite, states);
  target.draw(*mHeaderLabel, states);
  target.draw(*mButtonView, states);
}
